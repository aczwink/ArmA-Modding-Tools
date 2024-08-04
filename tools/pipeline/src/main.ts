#!/usr/bin/env node
/**
 * ArmA-Modding-Tools
 * Copyright (C) 2023-2024 Amir Czwink (amir130@hotmail.de)
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
import { BuildDtaExtStep, CompileConfigStep, CopyFilesStep, ExtractPBOStep, ImportFilesStep, IncludePipelineStepsStep, PackArchiveStep, PipelineDefinition, PipelineStep, RepackArchiveStep } from "./PipelineDefinition";
import { PBOFileCatalog } from "./PBOFileCatalog";
import { GameFileCatalog } from "./GameFileCatalog";
import { CallLocalBin, DeleteIfExisting, EnsureDirectoryExists, Exec, IntegrityCheck, IsNewer, __SetBinDir } from "./Helpers";
import { XMLParser } from "fast-xml-parser";
import { Dictionary } from "acts-util-core";
import { ArchiveFileProvider } from "./ArchiveFileProvider";

interface EnvironmentConfig
{
    archivesPath: string;
    md5Hashes: Dictionary<string>;
    pbosTempPath: string;
    pipelineDirPath: string;
    tempPath: string;
    vars: dotenv.DotenvParseOutput
}

async function RunBuildDtaExtJob(step: BuildDtaExtStep, env: EnvironmentConfig)
{
    function Capitalize(word: string)
    {
        return word.charAt(0).toUpperCase() + word.substring(1);
    }
    function PropertyToString(propertyName: string)
    {
        switch(propertyName)
        {
            case "caliber":
                return "Cal.";
            case "cyclicRate":
                return "Cyclic rate";
            case "effectiveCasualtyProducingRadius":
                return "Effective Casualty-Producing <br>Radius";
            case "fragmentationRadius":
                return "Fragmentation Radius";
            case "killRadius":
                return "Kill Radius";
            case "launcherCaliber":
                return "Launcher cal.";
            case "launcherWeight":
                return "Launcher weight";
            case "lengthClosed":
                return "Length (Closed)";
            case "lengthExtended":
                return "Length (Extended)";
            case "maxRange":
                return "Max. range";
            case "muzzleVelocity":
                return "Muzzle vel.";
        }
        return Capitalize(propertyName);
    }

    const idMapPath = path.join(env.vars[step.id_map.source], step.id_map.filePath);
    const idMapRaw = await fs.promises.readFile(idMapPath, "utf-8");
    const lines = idMapRaw.split("\n");
    const regEx = new RegExp(step.id_map.regExp);
    const idMap: Dictionary<string> = {};
    for (const line of lines)
    {
        const match = line.trim().match(regEx);
        if(match !== null)
        {
            idMap[match.groups!.from] = match.groups!.to;
        }
    }


    const sourcePath = env.vars[step.sourceLocation];

    const children = await fs.promises.readdir(sourcePath, "utf-8");
    const items = [];
    for (const child of children)
    {
        const raw = await fs.promises.readFile(path.join(sourcePath, child));
        const parser = new XMLParser();
        const parsed = parser.parse(raw);

        const parsedPath = path.parse(child);
        const type = ("weapon" in parsed) ? "weapon" : "item";
        items.push({
            id: parsedPath.name,
            type,
            ...parsed[type]
        });
    }

    let equipmentHtml = `
<html>

<head>
<meta http-equiv="Content-Type"
content="text/html; charset=windows-1250">
<title>Equip</title>
</head>
<body bgcolor="#FFFFFF">
`;

    for (const item of items)
    {
        const props = (typeof item.properties === "string") ? "" : item.properties.Entries().Map( (kv: any) => "<b>" + PropertyToString(kv.key) + ":</b> " + kv.value + "<br>").Join("\n");
        const id = idMap[item.id] ?? item.id;
        const img = (item.type === "item") ? ("@equip\\m\\m_" + id + ".paa") : ("@equip\\w\\w_" + id + ".paa");

        equipmentHtml += `
<h1 align="center"><a name="EQ_${id}"></a>${item.title}</h1>
<h2 align="center"><a name="EQ_${id}"></a>${item.category}</h2>
<br>
<p align="center"><img src="${img}" width="140" height="70"></p>

<h4><br>${item.description}</h4>

<h4><br><a href="#${id}_page2">Parameters</a></h4>
<address>
    <a href="#equipment"><img src="sipka_left.paa" border="0" width="20" height="20"></a> 
</address>
<hr>

<h1 align="center">${item.title}</h1>
<h2 align="center"><a name="${id}_page2"></a>Parameters</h2>
<br><h3 align="center"><img src="${img}" width="140"
height="70"></h3>

<h4>
<br>${props}
</h4>
<div align="right">

<address>
    <a href="#EQ_${id}"><img src="sipka_left.paa" border="0"
    width="20" height="20"></a> 
</address>
</div>
<hr>
        `;
    }

    equipmentHtml += "</body></html>";

    const dtaextTempPath = path.join(env.tempPath, "dtaext");
    const equipTempPath = path.join(dtaextTempPath, "equip");
    await EnsureDirectoryExists(equipTempPath);

    const magsPath = path.join(equipTempPath, "m");
    const weaponsPath = path.join(equipTempPath, "w");

    await EnsureDirectoryExists(magsPath);
    await EnsureDirectoryExists(weaponsPath);

    const langs = [".French", ".german", "", ".Italian", ".Spanish"];
    for (const lang of langs)
    {
        await fs.promises.writeFile(path.join(equipTempPath, "equipment" + lang + ".html"), equipmentHtml, "utf-8");
    }

    //images
    const archiveFileProvider = new ArchiveFileProvider(env.archivesPath);
    const pboCatalog = new PBOFileCatalog(archiveFileProvider, env.pbosTempPath, []);

    for (const imgSrc of step.imageSources)
    {
        const imgSourcePath = env.vars[imgSrc.sourceLocation];

        for (const pbo of imgSrc.pbos)
        {
            const sourceFilePath = path.join(imgSourcePath, imgSrc.name);
            IntegrityCheck(sourceFilePath, env.md5Hashes);
            pboCatalog.AddPBO(sourceFilePath, pbo);
        }

        for (const fileEntry of imgSrc.files)
        {
            const sourceFilePath = await pboCatalog.ProvideFile(fileEntry.sourceFileName);

            let targetFolderPath, prefix;
            switch(fileEntry.type)
            {
                case "base":
                    targetFolderPath = equipTempPath;
                    prefix = "";
                    break;
                case "magazine":
                    targetFolderPath = magsPath;
                    prefix = "m_";
                    break;
                case "weapon":
                    targetFolderPath = weaponsPath;
                    prefix = "w_";
                    break;
            }

            const mappedId = idMap[fileEntry.targetName] ?? fileEntry.targetName;
            const targetFilePath = path.join(targetFolderPath, prefix + mappedId + ".paa");

            await fs.promises.copyFile(sourceFilePath, targetFilePath);
            await fs.promises.chmod(targetFilePath, 0o664);
        }
    }

    await pboCatalog.UnmountAll();

    //pack pbo
    const targetPath = path.join(env.vars["target"], "Dta", "DTAEXT.PBO");
    await DeleteIfExisting(targetPath);
    await CallLocalBin("ArC", ["pack", dtaextTempPath, targetPath]);
}

async function RunCompileConfigJob(step: CompileConfigStep, env: EnvironmentConfig)
{
    const sourceFilePath = path.join(env.vars[step.source], step.sourceFile);
    const parsed = path.parse(sourceFilePath);
    const targetFilePath = path.join(env.vars["target"], step.targetFolder, parsed.name + ".bin");

    const isNewer = await IsNewer(path.dirname(sourceFilePath), targetFilePath);
    if(!isNewer)
        return;

    await EnsureDirectoryExists(path.dirname(targetFilePath));

    if(step.definitions === undefined)
        await CallLocalBin("raPEdit", [sourceFilePath, ">", targetFilePath]);
    else
    {
        const sourceDirPath = path.dirname(sourceFilePath);
        const tmpFilePath = path.join(sourceDirPath, "._tmp_arma_pipeline.cpp");

        const text = step.definitions.map(x => "#define " + x).join("\n") + '\n#include "' + path.basename(sourceFilePath) + '"';

        await fs.promises.writeFile(tmpFilePath, text, "utf-8");
        await CallLocalBin("raPEdit", [tmpFilePath, ">", targetFilePath]);
        await fs.promises.unlink(tmpFilePath);
    }
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

async function RunExtractPBOJob(step: ExtractPBOStep, env: EnvironmentConfig)
{
    const sourcePath = env.vars[step.source.sourceLocation];
    const sourceFilePath = (step.source.type === "Archive") ? path.join(sourcePath, step.source.name) : sourcePath;

    IntegrityCheck(sourceFilePath, env.md5Hashes);

    const targetName = path.basename(step.source.pbo);
    const targetPath = path.join(env.vars["target"], step.targetFolder, targetName);

    const isNewer = await IsNewer(sourceFilePath, targetPath);
    if(isNewer)
    {
        const archiveFileProvider = new ArchiveFileProvider(env.archivesPath);
        const pboPath = await archiveFileProvider.ProvideFile(sourceFilePath, step.source.pbo);

        await DeleteIfExisting(targetPath);
        await fs.promises.copyFile(pboPath, targetPath);
    }
}

async function RunImportFilesJob(step: ImportFilesStep, env: EnvironmentConfig)
{
    const tempPath = env.tempPath;
    const targetPath = env.vars["target"];

    const jobsTempPath = path.join(tempPath, "jobs");
    await EnsureDirectoryExists(jobsTempPath);

    const packTempPath = path.join(tempPath, "pack");
    await EnsureDirectoryExists(packTempPath);

    const archiveFileProvider = new ArchiveFileProvider(env.archivesPath);
    const pboCatalog = new PBOFileCatalog(archiveFileProvider, env.pbosTempPath, step.ignore);
    const fileCatalog = new GameFileCatalog(jobsTempPath, packTempPath, pboCatalog);
    for (const source of step.sources)
    {
        switch(source.type)
        {
            case "Archive":
            {
                for (const pbo of source.pbos)
                {
                    const sourcePath = env.vars[source.sourceLocation];
                    const sourceFilePath = path.join(sourcePath, source.name);
                    IntegrityCheck(sourceFilePath, env.md5Hashes);
                    pboCatalog.AddPBO(sourceFilePath, pbo);
                }
            }
            break;
            case "FileSystemPBO":
            {
                const sourcePath = env.vars[source.sourceLocation];
                const sourceFilePath = path.join(sourcePath, source.pbo);
                IntegrityCheck(sourceFilePath, env.md5Hashes);
                pboCatalog.AddPBO(sourcePath, source.pbo);
            }
            break;
        }
    }

    for (const source of step.sources)
    {
        switch(source.type)
        {
            case "Archive":
            case "FileSystemPBO":
            {
                const files = Array.isArray(source.files) ? source.files : [source.files];
                for (const file of files)
                {
                    const sourceGamePath = (typeof file === "string") ? file : file.sourceFileName;
                    const targetName = (typeof file === "string") ? undefined : file.targetName;

                    const sourcePath = await pboCatalog.ProvideFile(sourceGamePath);
                    await fileCatalog.MarkForImport(sourceGamePath, sourcePath!, targetName);

                    if( (typeof file !== "string") && (file.patch !== undefined) )
                        fileCatalog.SetPatchData(sourceGamePath, file.patch);
                }
            }
            break;
            case "FileSystem":
            {
                const sourcePath = env.vars[source.sourceLocation];
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

async function RunIncludePipelineStepsJob(step: IncludePipelineStepsStep, env: EnvironmentConfig)
{
    const pipelinePath = path.join(env.pipelineDirPath, step.name);
    const pipelineCode = await fs.promises.readFile(pipelinePath, "utf-8");
    const pipelineDef = YAML.parse(pipelineCode) as PipelineDefinition;

    for (const step of pipelineDef.steps)
    {
        await RunStep(step, env);
    }
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
    const sourcePath = env.vars[step.source.sourceLocation];
    const sourcePBOPath = (step.source.type === "Archive") ? path.join(sourcePath, step.source.name) : sourcePath;

    const repackPath = path.join(env.tempPath, "repack");
    await EnsureDirectoryExists(repackPath);

    const archiveFileProvider = new ArchiveFileProvider(env.archivesPath);
    const pboCatalog = new PBOFileCatalog(archiveFileProvider, env.pbosTempPath, []);
    const pboPath = await pboCatalog.MountPBO(sourcePBOPath, step.source.pbo);

    const targetPath = path.join(repackPath, step.targetPboName);
    await EnsureDirectoryExists(targetPath);

    const excludes = step.exclude.Values().Map(x => ["--exclude",  x].Values()).Flatten();
    await Exec(["rsync", "-a", "--chmod=+rwX", pboPath + "/", targetPath, ...excludes]);

    for (const entry of step.include)
    {
        const fileEntries = Array.isArray(entry.files) ? entry.files : [entry.files];
        for (const fileEntry of fileEntries)
        {
            const filePath = (typeof fileEntry === "string") ? fileEntry : fileEntry.sourceFileName;
            let sourceFilePath;
            switch(entry.type)
            {
                case "Archive":
                    throw new Error("NOT IMPLEMENTED");
                case "FileSystem":
                    sourceFilePath = path.join(env.vars[entry.sourceLocation], filePath);
                    break;
                case "FileSystemPBO":
                    throw new Error("TODO: implement me");
                case "FlatArchive":
                    sourceFilePath = await archiveFileProvider.ProvideFile(path.join(env.vars[entry.sourceLocation], entry.name), filePath);
                    break;
            }

            const relativeTargetPath = (typeof fileEntry === "string") ? path.basename(filePath) : fileEntry.targetFileName;
            const targetFilePath = path.join(targetPath, relativeTargetPath);

            if(path.extname(sourceFilePath) === ".cpp")
            {
                const parsed = path.parse(targetFilePath);
                await CallLocalBin("raPEdit", [sourceFilePath, ">", path.join(parsed.dir, parsed.name + ".bin")]);
            }
            else
                await fs.promises.copyFile(sourceFilePath, targetFilePath);   
        }
    }

    const packPath = path.join(env.vars["target"], step.targetPboName + ".pbo");

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
        case "BuildDtaExt":
            await RunBuildDtaExtJob(step, env);
            break;
        case "CompileConfig":
            await RunCompileConfigJob(step, env);
            break;
        case "CopyFiles":
            await RunCopyFilesJob(step, env.vars);
            break;
        case "Create7zArchive":
            await RunCreate7zArchiveJob(env.vars);
            break;
        case "ExtractPBO":
            await RunExtractPBOJob(step, env);
            break;
        case "ImportFiles":
            await RunImportFilesJob(step, env);
            break;
        case "IncludePipelineSteps":
            await RunIncludePipelineStepsJob(step, env);
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
        md5Hashes: {},
        pipelineDirPath: path.dirname(pipelinePath),
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

    envConfig.md5Hashes = pipelineDef.source_integrity;

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