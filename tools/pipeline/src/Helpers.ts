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
import { exec } from "child_process";
import fs from "fs";
import path from "path";

var g_binDir: string;
export function __SetBinDir(dir: string)
{
    g_binDir = dir;
}

export async function CallLocalBin(binary: string, args: string[])
{
    return await Exec(["./" + binary, ...args], g_binDir, {
        LD_LIBRARY_PATH: g_binDir
    });
}

export async function DeleteIfExisting(path: string)
{
    try
    {
        await fs.promises.rm(path, { recursive: true });
    }
    catch(e: any)
    {
        if(e.code === "ENOENT")
            return;
        throw e;
    }
}

export async function EnsureDirectoryExists(dirPath: string)
{
    const exists = fs.existsSync(dirPath);
    if(exists)
        return;
    await EnsureDirectoryExists(path.dirname(dirPath));
    await fs.promises.mkdir(dirPath);
}

export function Exec(command: string[], workingDirectory?: string, env?: NodeJS.ProcessEnv)
{
    function EscapeArg(arg: string)
    {
        return arg
            .ReplaceAll("\\", "\\\\")
            .ReplaceAll(" ", "\\ ");
    }

    const cmdline = command.map(EscapeArg).join(" ");
    return new Promise<string>( (resolve, reject) => {
        exec(cmdline, {
            cwd: workingDirectory,
            env,
            maxBuffer: 10 * 1024 * 1024,
        }, (error, stdout, stderr) => {
            if(error !== null)
                reject(error);
            else
                resolve(stdout);
        });
    });
}

export async function IsNewer(sourcePath: string, targetPath: string)
{
    async function QueryPathChangeDate(path: string)
    {
        try
        {
            const stats = await fs.promises.stat(path);
            return stats.mtimeMs;
        }
        catch(e: any)
        {
            if(e.code === "ENOENT")
                return -1;
            throw e;
        }
    }

    const a = await QueryPathChangeDate(sourcePath);
    const b = await QueryPathChangeDate(targetPath);

    return a > b;
}