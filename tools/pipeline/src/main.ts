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
import { CopyFilesStep, PipelineDefinition, PipelineStep } from "./PipelineDefinition";
import { execSync } from "child_process";

async function EnsureDirectoryExists(dirPath: string)
{
    const exists = fs.existsSync(dirPath);
    if(exists)
        return;
    await EnsureDirectoryExists(path.dirname(dirPath));
    await fs.promises.mkdir(dirPath);
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

        if(fs.existsSync(targetFilePath))
            continue;
        if(fs.statSync(sourceFilePath).isDirectory())
        {
            execSync("cp -r " + sourceFilePath + " " + targetFilePath);
            continue;
        }

        await EnsureDirectoryExists(path.dirname(targetFilePath));
        await fs.promises.copyFile(sourceFilePath, targetFilePath);
    }
}

function RunCreateArchiveJob(env: dotenv.DotenvParseOutput)
{
    const targetPath = env["target"];

    const compressionStrength = 6; //9 highest
    const cmdline = "7z a -t7z -mmt=on -mx=" + compressionStrength + " " + targetPath + ".7z " + targetPath + "/*";
    execSync(cmdline);
}

async function RunStep(step: PipelineStep, env: dotenv.DotenvParseOutput)
{
    switch(step.type)
    {
        case "CopyFiles":
            await RunCopyFilesJob(step, env);
            break;
        case "CreateArchive":
            await RunCreateArchiveJob(env);
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

async function LoadAndRunPipeline()
{
    const pipelinePath = process.argv[2];
    const envPath = process.argv[3];

    const envData = await fs.promises.readFile(envPath, "utf-8");
    const env = dotenv.parse(envData);

    const pipelineCode = await fs.promises.readFile(pipelinePath, "utf-8");
    const pipelineDef = YAML.parse(pipelineCode) as PipelineDefinition;

    const totalStart = Date.now();

    for(let i = 0; i < pipelineDef.steps.length; i++)
    {
        const start = Date.now();

        console.log("Running step " + (i+1) + ": " + pipelineDef.steps[i].type);
        await RunStep(pipelineDef.steps[i], env);

        const end = Date.now();
        const delta = (end - start) / 1000;
        console.log(`Execution time for step: ${DurationToString(delta)}`);
    }

    const totalEnd = Date.now();
    const delta = (totalEnd - totalStart) / 1000;
    console.log(`Total execution time: ${DurationToString(delta)}`);
}

LoadAndRunPipeline();