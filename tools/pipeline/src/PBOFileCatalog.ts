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
import fs from "fs";
import path from "path";
import { Dictionary, TimeUtil } from "acts-util-core";
import { Lock } from "acts-util-node";
import { CallLocalBin, EnsureDirectoryExists, Exec } from "./Helpers";
import { CaseInsensitiveCacheNode } from "./CaseInsensitiveCacheNode";

interface PBOInfo
{
    archiveFilePath: string;
    isMounted: boolean;
    pboPath: string;
}

interface PBOCasingInfo
{
    realCasing: string;
    node: CaseInsensitiveCacheNode | null;
}

export class PBOFileCatalog
{
    constructor(private archivesTempPath: string, private pbosTempPath: string, private ignoreReportingForPBOs: string[])
    {
        this.pbos = {};
        this.pboCasing = {};
        this.pboLock = new Lock;
    }

    //Public methods
    public AddPBO(archiveFilePath: string, pboPath: string)
    {
        const pboName = path.parse(pboPath).name;
        if(pboName in this.pbos)
            throw new Error("Method not implemented.");
        this.pbos[pboName] = {
            archiveFilePath,
            isMounted: false,
            pboPath
        };
        this.pboCasing[pboName.toLowerCase()] = {
            realCasing: pboName,
            node: null,
        };
    }

    public async MountPBO(archiveFilePath: string, pboPath: string)
    {
        const pboName = path.parse(pboPath).name;
        
        this.AddPBO(archiveFilePath, pboPath);
        const pboInfo = this.pbos[pboName]!;
        const extractedPBOPath = await this.EnsurePBOFileIsExtracted(pboInfo);
        await this.EnsurePBOFileIsMounted(extractedPBOPath, pboName, pboInfo);

        return this.GetMountedPBOPath(pboName);
    }

    public async ProvideFile(gamePath: string)
    {
        const pboName = gamePath.split("/")[0];
        const pboInfo = this.pbos[pboName]!;

        const extractedPBOPath = await this.EnsurePBOFileIsExtracted(pboInfo);
        await this.EnsurePBOFileIsMounted(extractedPBOPath, pboName, pboInfo);
        return this.GamePathToFileSystemPath(gamePath);
    }

    public async ResolveCaseInsensitivePath(caseInsensitivePath: string): Promise<string | undefined>
    {
        const segments = caseInsensitivePath.split("/");
        const pboNameWrongCase = segments[0];
        const casingInfo = this.pboCasing[pboNameWrongCase.toLowerCase()];

        if(casingInfo === undefined)
        {
            if(!this.ignoreReportingForPBOs.Contains(pboNameWrongCase.toLowerCase()))
                console.error("Missing file: " + caseInsensitivePath);
            return undefined;
        }
        if(casingInfo.node === null)
        {
            const pboInfo = this.pbos[casingInfo.realCasing]!;
            const extractedPBOPath = await this.EnsurePBOFileIsExtracted(pboInfo);
            await this.EnsurePBOFileIsMounted(extractedPBOPath, casingInfo.realCasing, pboInfo);
        }

        const result = casingInfo.node!.Match(segments.slice(1));
        if(result === undefined)
            return undefined;
        return path.join(casingInfo.realCasing, result);
    }

    public async UnmountAll()
    {
        for (const pboName in this.pbos)
        {
            if (Object.prototype.hasOwnProperty.call(this.pbos, pboName))
            {
                const pboInfo = this.pbos[pboName]!;
                if(!pboInfo.isMounted)
                    continue;
                
                const mountedPBOPath = this.GetMountedPBOPath(pboName);
                await Exec(["fusermount", "-u", mountedPBOPath]);
            }
        }
    }

    //Private state
    private pbos: Dictionary<PBOInfo>;
    private pboCasing: Dictionary<PBOCasingInfo>;
    private pboLock: Lock;

    //Private methods
    private async EnsurePBOFileIsExtracted(pboInfo: PBOInfo)
    {
        const extractedArchivePathCompressed = path.join(this.archivesTempPath, pboInfo.pboPath);
        const parts = extractedArchivePathCompressed.split(".");
        while(parts[parts.length - 1].toLowerCase() !== "pbo")
            parts.Remove(parts.length - 1);
        const extractedArchivePath = parts.join(".");

        //const releaser = await this.pboLock.Lock();
        if(!fs.existsSync(extractedArchivePath))
        {
            await this.ExtractPBOFromArchive(pboInfo);
        }
        //releaser.Release();

        return extractedArchivePath;
    }

    private async EnsurePBOFileIsMounted(extractedPBOPath: string, pboName: string, pboInfo: PBOInfo)
    {
        const mountPath = path.join(this.pbosTempPath, pboName);
        if(pboInfo.isMounted)
            return;

        await EnsureDirectoryExists(mountPath);

        CallLocalBin("ArC", ["mount", extractedPBOPath, mountPath]);
        await TimeUtil.Delay(1000);

        this.pboCasing[pboName.toLowerCase()]!.node = await CaseInsensitiveCacheNode.Build(mountPath);

        pboInfo.isMounted = true;
    }

    private async ExtractPBOFromArchive(pboInfo: PBOInfo)
    {
        switch(path.extname(pboInfo.archiveFilePath).substring(1))
        {
            case "7z":
            case "cab":
            case "zip":
                await Exec(["7z", "x", "-aos", "-i!" + pboInfo.pboPath, pboInfo.archiveFilePath], this.archivesTempPath);
                break;
            case "exe":
            {
                try
                {
                    await Exec(["7z", "x", "-aos", "-i!" + pboInfo.pboPath, pboInfo.archiveFilePath], this.archivesTempPath);
                }
                catch(e: any)
                {
                    if(e.code === 2)
                    {
                        await Exec(["unrar", "x", "-o+", pboInfo.archiveFilePath, pboInfo.pboPath], this.archivesTempPath);
                    }
                    else
                        throw e;
                }
            }
            break;
            case "rar":
                await Exec(["unrar", "x", "-o+", pboInfo.archiveFilePath, pboInfo.pboPath], this.archivesTempPath);
                break;
            default:
                throw new Error("ExtractPBOFromArchive: " + path.extname(pboInfo.archiveFilePath));
        }

        await this.UncompressPBO(pboInfo.pboPath);
    }

    private GamePathToFileSystemPath(gamePath: string)
    {
        return path.join(this.pbosTempPath, gamePath)
    }

    private GetMountedPBOPath(pboName: string)
    {
        return path.join(this.pbosTempPath, pboName);
    }

    private async UncompressPBO(compressedFilePath: string)
    {
        const extName = path.extname(compressedFilePath);
        switch(extName.toLowerCase())
        {
            case ".pbo":
                break;
            case ".xz":
                await Exec(["xz", "-d", "-S", extName, compressedFilePath], this.archivesTempPath);
                break;
            default:
                throw new Error("Method not implemented: " + extName);
        }
    }
}
