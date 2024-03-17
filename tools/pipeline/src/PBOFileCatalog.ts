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
import { ArchiveFileProvider } from "./ArchiveFileProvider";

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
    constructor(private archiveFileProvider: ArchiveFileProvider, private pbosTempPath: string, private ignoreReportingForPBOs: string[])
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
                await this.UnmountPBOWithRetries(mountedPBOPath);
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
        const pboPath = path.join(pboInfo.archiveFilePath, pboInfo.pboPath);
        if(fs.existsSync(pboPath))
            return pboPath;
        
        const extractedArchivePath = await this.archiveFileProvider.ProvideFile(pboInfo.archiveFilePath, pboInfo.pboPath);
        return extractedArchivePath;
    }

    private async EnsurePBOFileIsMounted(extractedPBOPath: string, pboName: string, pboInfo: PBOInfo)
    {
        const mountPath = path.join(this.pbosTempPath, pboName);
        if(pboInfo.isMounted)
            return;

        const releaser = await this.pboLock.Lock();
        if(pboInfo.isMounted)
        {
            releaser.Release();
            return;
        }

        await EnsureDirectoryExists(mountPath);

        CallLocalBin("ArC", ["mount", extractedPBOPath, mountPath]);
        await TimeUtil.Delay(1000);

        this.pboCasing[pboName.toLowerCase()]!.node = await CaseInsensitiveCacheNode.Build(mountPath);

        pboInfo.isMounted = true;

        releaser.Release();
    }

    private GamePathToFileSystemPath(gamePath: string)
    {
        return path.join(this.pbosTempPath, gamePath)
    }

    private GetMountedPBOPath(pboName: string)
    {
        return path.join(this.pbosTempPath, pboName);
    }

    private async UnmountPBO(mountedPBOPath: string)
    {
        try
        {
            await Exec(["fusermount", "-u", mountedPBOPath]);
        }
        catch(e: any)
        {
            if(e.code === 1)
                return false;
            throw e;
        }
        return true;
    }

    private async UnmountPBOWithRetries(mountedPBOPath: string)
    {
        for(let i = 0; i < 5; i++)
        {
            const result = await this.UnmountPBO(mountedPBOPath);
            if(result)
                return;
            await TimeUtil.Delay(1000);
        }
        throw new Error("TODO: COULD NOT UNMOUNT");
    }
}
