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
import { Lock } from "acts-util-node";
import fs from "fs";
import path from "path";
import { Exec } from "./Helpers";

export class ArchiveFileProvider
{
    constructor(private extractArchivesTempPath: string)
    {
        this.extractLock = new Lock;
    }

    //Public methods
    public async ProvideFile(archiveFilePath: string, insideArchivePath: string)
    {
        const trimmed = this.TrimCompressionExtensions(insideArchivePath);
        const extractedPath = path.join(this.extractArchivesTempPath, trimmed);

        const releaser = await this.extractLock.Lock();
        if(!fs.existsSync(extractedPath))
        {
            await this.ExtractFileFromArchive(archiveFilePath, insideArchivePath);
        }
        releaser.Release();

        return extractedPath;
    }

    //Private state
    private extractLock: Lock;

    //Private methods
    private async ExtractFileFromArchive(archiveFilePath: string, insideArchivePath: string)
    {
        switch(path.extname(archiveFilePath).substring(1))
        {
            case "7z":
            case "cab":
            case "zip":
                await Exec(["7z", "x", "-aos", "-i!" + insideArchivePath, archiveFilePath], this.extractArchivesTempPath);
                break;
            case "exe":
            {
                try
                {
                    await Exec(["7z", "x", "-aos", "-i!" + insideArchivePath, archiveFilePath], this.extractArchivesTempPath);
                }
                catch(e: any)
                {
                    if(e.code === 2)
                    {
                        await Exec(["unrar", "x", "-o+", archiveFilePath, insideArchivePath], this.extractArchivesTempPath);
                    }
                    else
                        throw e;
                }
            }
            break;
            case "rar":
                try
                {
                    await Exec(["unrar", "x", "-o+", archiveFilePath, insideArchivePath], this.extractArchivesTempPath);
                }
                catch(e: any)
                {
                    if(e.code === 6)
                    {
                        //for some rars somehow only full extraction works -.-
                        await Exec(["unrar", "x", "-o+", archiveFilePath], this.extractArchivesTempPath);
                    }
                    else
                        throw e;
                }
                break;
            default:
                throw new Error("ExtractPBOFromArchive: " + archiveFilePath + ", " + insideArchivePath);
        }

        await this.UncompressFile(insideArchivePath);
    }

    private TrimCompressionExtensions(untrimmedPath: string)
    {
        const parts = untrimmedPath.split(".");
        if(parts[parts.length - 1].toLowerCase() === "xz")
            parts.Remove(parts.length - 1);
        return parts.join(".");
    }

    private async UncompressFile(compressedFilePath: string)
    {
        const extName = path.extname(compressedFilePath);
        switch(extName.toLowerCase())
        {
            case ".xz":
                await Exec(["xz", "-d", "-S", extName, compressedFilePath], this.extractArchivesTempPath);
                break;
        }
    }
}
