#!/usr/bin/env node
/**
 * ArmA-Modding-Tools
 * Copyright (C) 2023 Amir Czwink (amir130@hotmail.de)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
import dotenv from "dotenv";
import fs from "fs";
import { glob } from "glob";
import path from "path";
import YAML from 'yaml'
import { CompileConfigStep, CopyFilesStep, ImportFilesStep, PackArchiveStep, PipelineDefinition, PipelineStep, RepackArchiveStep } from "./PipelineDefinition";
import { PBOFileCatalog } from "./PBOFileCatalog";
import { GameFileCatalog } from "./GameFileCatalog";
import { CallLocalBin, DeleteIfExisting, EnsureDirectoryExists, Exec, IsNewer, __SetBinDir } from "./Helpers";

interface EnvironmentConfig
{
    archivesPath: string;
    pbosTempPath: string;
    tempPath: string;
    vars: dotenv.DotenvParseOutput
}

async function RunCompileConfigJob(step: CompileConfigStep, env: EnvironmentConfig)
{
    const sourceFilePath = path.join(env.vars[step.source], step.sourceFile);
    const parsed = path.parse(sourceFilePath);
    const targetFilePath = path.join(env.vars["target"], step.targetFolder, parsed.name + ".bin");

    await EnsureDirectoryExists(path.dirname(targetFilePath));
    if(await IsNewer(path.dirname(sourceFilePath), targetFilePath))
        await CallLocalBin("raPEdit", [sourceFilePath, ">", targetFilePath]);
}

async function RunCopyFilesJob(step: CopyFilesStep, env: dotenv.DotenvParseOutput)
{
    const sourcePath = env[step.source];
    const targetPath = env["target"];

    const files = Array.isArray(step.files) ? step.files : [step.files];
    const sourcePathPatterns = files.map(x => path.join(sourcePath, x));

    const sourceFilePaths = await glob(sourcePathPatterns);

    for (const sourceFilePath of sourceFilePaths)
    {
        const relativePath = path.relative(sourcePath, sourceFilePath);
        const targetFilePath = path.join(targetPath, relativePath);

        const isNewer = await IsNewer(sourceFilePath, targetFilePath);
        if(!isNewer)
            continue;
        if(fs.statSync(sourceFilePath).isDirectory())
        {
            await Exec(["cp", "-r", sourceFilePath, targetFilePath]);
            continue;
        }

        await EnsureDirectoryExists(path.dirname(targetFilePath));
        await fs.promises.copyFile(sourceFilePath, targetFilePath);
    }
}

async function RunCreate7zArchiveJob(env: dotenv.DotenvParseOutput)
{
    const targetPath = env["target"];

    const compressionStrength = env["compressionStrength"]; //9 highest
    await Exec(["7z", "a", "-t7z", "-mmt=on", "-mx=" + compressionStrength, targetPath + ".7z", targetPath + "/*"]);
}

async function RunImportFilesJob(step: ImportFilesStep, env: EnvironmentConfig)
{
    const sourcePath = env.vars[step.source];
    const tempPath = env.tempPath;
    const targetPath = env.vars["target"];

    const jobsTempPath = path.join(tempPath, "jobs");
    await EnsureDirectoryExists(jobsTempPath);

    const packTempPath = path.join(tempPath, "pack");
    await EnsureDirectoryExists(packTempPath);

    const pboCatalog = new PBOFileCatalog(env.archivesPath, env.pbosTempPath, step.ignore);
    const fileCatalog = new GameFileCatalog(jobsTempPath, packTempPath, pboCatalog);
    for (const source of step.sources)
    {
        switch(source.type)
        {
            case "Archive":
            {
                for (const pbo of source.pbos)
                    pboCatalog.AddPBO(path.join(sourcePath, source.name), pbo);
            }
            break;
        }
    }

    for (const source of step.sources)
    {
        switch(source.type)
        {
            case "Archive":
            {
                for (const file of source.files)
                {
                    const sourceGamePath = (typeof file === "string") ? file : file.sourceFileName;
                    const targetName = (typeof file === "string") ? undefined : file.targetName;

                    const sourcePath = await pboCatalog.ProvideFile(sourceGamePath);
                    await fileCatalog.MarkForImport(sourceGamePath, sourcePath!, targetName);
                }
            }
            break;
            case "FileSystem":
            {
                const sourcePath = env.vars[source.source];
                for (const file of source.files)
                {
                    await fileCatalog.MarkForImport(file, path.join(sourcePath, file));
                }
            }
            break;
        }
    }
    fileCatalog.RenameUnmarked();
    await fileCatalog.ImportMarked();
    await fileCatalog.PackArchives(path.join(targetPath, "Dta"));

    await pboCatalog.UnmountAll();
}

async function RunPackArchiveJob(step: PackArchiveStep, env: dotenv.DotenvParseOutput)
{
    const sourcePath = path.join(env[step.source], step.source_folder);
    const targetName = step.targetFileName ?? path.basename(step.source_folder);
    const targetPath = path.join(env["target"], step.target_folder, targetName + ".pbo");

    const isNewer = await IsNewer(sourcePath, targetPath);
    if(isNewer)
    {
        await DeleteIfExisting(targetPath);
        await CallLocalBin("ArC", ["pack", sourcePath, targetPath]);
    }
}

async function RunRepackArchiveJob(step: RepackArchiveStep, env: EnvironmentConfig)
{
    const sourcePath = env.vars[step.sourceLocation];

    const repackPath = path.join(env.tempPath, "repack");
    await EnsureDirectoryExists(repackPath);

    const pboCatalog = new PBOFileCatalog(env.archivesPath, env.pbosTempPath, []);
    const pboPath = await pboCatalog.MountPBO(path.join(sourcePath, step.source.name), step.source.pbo);

    const targetPath = path.join(repackPath, step.targetPboName);
    await EnsureDirectoryExists(targetPath);

    const excludes = step.exclude.Values().Map(x => ["--exclude",  x].Values()).Flatten();
    await Exec(["rsync", "-a", "--chmod=+rwX", pboPath + "/", targetPath, ...excludes]);

    for (const entry of step.include)
    {
        const sourceFilePath = path.join(env.vars[entry.source], entry.path);
        const targetFilePath = path.join(targetPath, path.basename(entry.path));

        if(path.extname(sourceFilePath) === ".cpp")
        {
            const parsed = path.parse(targetFilePath);
            await CallLocalBin("raPEdit", [sourceFilePath, ">", path.join(parsed.dir, parsed.name + ".bin")]);
        }
        else
            await fs.promises.copyFile(sourceFilePath, targetFilePath);
    }

    const packPath = path.join(env.vars["target"], "AddOns", step.targetPboName + ".pbo");

    if(!fs.existsSync(packPath))
    {
        await EnsureDirectoryExists(path.dirname(packPath));
        await CallLocalBin("ArC", ["pack", targetPath, packPath]);
    }

    await pboCatalog.UnmountAll();
}

async function RunStep(step: PipelineStep, env: EnvironmentConfig)
{
    switch(step.type)
    {
        case "CompileConfig":
            await RunCompileConfigJob(step, env);
            break;
        case "CopyFiles":
            await RunCopyFilesJob(step, env.vars);
            break;
        case "Create7zArchive":
            await RunCreate7zArchiveJob(env.vars);
            break;
        case "ImportFiles":
            await RunImportFilesJob(step, env);
            break;
        case "PackArchive":
            await RunPackArchiveJob(step, env.vars);
            break;
        case "RepackArchive":
            await RunRepackArchiveJob(step, env);
            break;
        default:
            throw new Error("Unknown pipeline step: " + YAML.stringify(step));
    }
}

function DurationToString(d: number)
{
    if(d >= 60)
    {
        const min = Math.floor(d / 60);
        const secs = Math.round(d % 60);

        return min + " min " + secs + " s";
    }

    return d + " seconds";
}

async function EnsureDependenciesAreDownloaded(binaryPath: string)
{
    if(!fs.existsSync(path.join(binaryPath)))
    {
        await EnsureDirectoryExists(binaryPath);

        await Exec(["wget", "https://github.com/aczwink/ArmA-Modding-Tools/releases/download/latest/bin_linux_x86-64.zip"], binaryPath);
        await Exec(["unzip", "bin_linux_x86-64.zip"], binaryPath);
        fs.promises.unlink(path.join(binaryPath, "bin_linux_x86-64.zip"));

        await Exec(["wget", "https://github.com/aczwink/ACFSLib/releases/download/latest/bin_linux_x86-64.zip"], binaryPath);
        await Exec(["unzip", "bin_linux_x86-64.zip"], binaryPath);
        fs.promises.unlink(path.join(binaryPath, "bin_linux_x86-64.zip"));

        await Exec(["wget", "https://github.com/aczwink/StdPlusPlus/releases/download/latest/bin_linux_x86-64.zip"], binaryPath);
        await Exec(["unzip", "bin_linux_x86-64.zip"], binaryPath);
        fs.promises.unlink(path.join(binaryPath, "bin_linux_x86-64.zip"));
    }
    __SetBinDir(binaryPath);
}

async function LoadAndRunPipeline()
{
    const pipelinePath = process.argv[2];
    const envPath = process.argv[3];

    //bootstrap
    const envData = await fs.promises.readFile(envPath, "utf-8");
    const env = dotenv.parse(envData);

    const tempPath = env["temp"];
    const envConfig: EnvironmentConfig = {
        archivesPath: path.join(tempPath, "archives"),
        pbosTempPath: path.join(tempPath, "pbos"),
        tempPath: tempPath,
        vars: env,
    };

    await EnsureDependenciesAreDownloaded(path.join(tempPath, "bin"));
    await EnsureDirectoryExists(envConfig.archivesPath);
    await EnsureDirectoryExists(envConfig.pbosTempPath);

    //load pipeline
    const pipelineCode = await fs.promises.readFile(pipelinePath, "utf-8");
    const pipelineDef = YAML.parse(pipelineCode) as PipelineDefinition;

    const totalStart = Date.now();

    for(let i = 0; i < pipelineDef.steps.length; i++)
    {
        const start = Date.now();

        console.log("Running step " + (i+1) + ": " + pipelineDef.steps[i].type);
        await RunStep(pipelineDef.steps[i], envConfig);

        const end = Date.now();
        const delta = (end - start) / 1000;
        console.log(`Execution time for step: ${DurationToString(delta)}`);
    }

    const totalEnd = Date.now();
    const delta = (totalEnd - totalStart) / 1000;
    console.log(`Total execution time: ${DurationToString(delta)}`);
}

LoadAndRunPipeline();