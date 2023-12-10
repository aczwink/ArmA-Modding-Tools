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
import { CallLocalBin, DeleteIfExisting, EnsureDirectoryExists, IsNewer } from "./Helpers";
import { Dictionary } from "acts-util-core";
import { PBOFileCatalog } from "./PBOFileCatalog";
import { pseudoRandomBytes } from "crypto";
import { P3DLod } from "./ArmAModdingTools";

interface FileMetaData
{
    dependencies: string[];
    marked: boolean;
    sourcePath: string;
    newName?: string;
    patchData?: any;
}

export class GameFileCatalog
{
    constructor(private jobsTempPath: string, private tempPath: string, private pboFileCatalog: PBOFileCatalog)
    {
        this.files = {};
        this.extCounters = {};
    }

    //Public methods
    public async ImportMarked()
    {
        for (const gamePath in this.files)
        {
            if (Object.prototype.hasOwnProperty.call(this.files, gamePath))
                await this.ImportFile(gamePath);
        }
    }

    public async MarkForImport(gamePath: string, sourcePath: string, targetName?: string)
    {
        await this.MarkForImportImpl(gamePath, sourcePath, true);
        if(targetName !== undefined)
            this.files[gamePath]!.newName = targetName;
    }

    public async PackArchives(targetPath: string)
    {
        await EnsureDirectoryExists(targetPath);
        
        const children = await fs.promises.readdir(this.tempPath);
        for (const child of children)
        {
            const sourcePath = path.join(this.tempPath, child);
            const targetFilePath = path.join(targetPath, child + ".pbo");
            if(await IsNewer(sourcePath, targetFilePath))
            {
                await DeleteIfExisting(targetFilePath);
                await CallLocalBin("ArC", ["pack", sourcePath, targetFilePath]);
            }
        }
    }

    public RenameUnmarked()
    {
        for (const gamePath in this.files)
        {
            if (Object.prototype.hasOwnProperty.call(this.files, gamePath))
            {
                const metadata = this.files[gamePath]!;
                if(!metadata.marked)
                {
                    const extension = path.extname(gamePath).substring(1);
                    const c = this.extCounters[extension];
                    if(c === undefined)
                    {
                        metadata.newName = "0";
                        this.extCounters[extension] = 1;
                    }
                    else
                    {
                        metadata.newName = c.toString();
                        this.extCounters[extension]!++;
                    }
                }
            }
        }
    }

    public SetPatchData(gamePath: string, patchData: any)
    {
        this.files[gamePath]!.patchData = patchData;
    }

    //Private state
    private files: Dictionary<FileMetaData>;
    private extCounters: Dictionary<number>;

    //Private methods
    private async AnalyzeDependencies(sourcePath: string): Promise<Set<string>>
    {
        const extension = path.extname(sourcePath).substring(1).toLowerCase();
        switch(extension)
        {
            case "ogg":
            case "paa":
            case "pac":
            case "rtm":
            case "wav":
            case "wss":
                return new Set();
            case "p3d":
                const data = await CallLocalBin("p3dEdit", [sourcePath, "json"]);
                const lods = JSON.parse(data).lods as P3DLod[];

                return lods.Values().Map(x => x.faces.Values()).Flatten().Map(x => x.texturePath.ReplaceAll("\\", "/")).Filter(x => x.length > 0).ToSet();
            case "wrp":
            {
                const data = await CallLocalBin("wrpEdit", [sourcePath, "json"]);
                const parsed = JSON.parse(data)["objects"] as any[];
                const modelFilePaths = new Set(parsed.map(x => (x.modelFilePath as string).ReplaceAll("\\", "/")));

                return modelFilePaths;
            }
            default:
                throw new Error("AnalyzeDependencies: " + extension);
        }
    }

    private BuildDependencyMap(dependencies: string[])
    {
        const replacements = [];

        for (const dependency of dependencies)
        {
            const replacement = this.BuildTargetGamePath(dependency);
            if(dependency === replacement)
                continue;
            replacements.push({ from: dependency.ReplaceAll("/", "\\"), to: replacement.ReplaceAll("/", "\\") });
        }

        return replacements;
    }

    private BuildTargetGamePath(gamePath: string)
    {
        function ExtractIntermediatePaths()
        {
            const parts = gamePath.split("/");
            return parts.slice(1, parts.length - 1).join("/");
        }

        const parsed = path.parse(gamePath);
        const metadata = this.files[gamePath]!;

        const ext = parsed.ext.substring(1);
        let folderName = this.MapExtensionToFolderName(ext);

        if( (metadata.newName === undefined) && ((ext === "paa") || (ext === "pac")) )
        {
            const intermediate = ExtractIntermediatePaths();
            if(intermediate.length > 0)
                folderName = path.join(folderName, intermediate);
        }
        const fileName = (metadata.newName ?? parsed.name) + parsed.ext;

        return path.join(folderName, fileName);
    }

    private async ImportFile(gamePath: string)
    {
        const targetGamePath = this.BuildTargetGamePath(gamePath);
        const targetPath = path.join(this.tempPath, targetGamePath);
        const metadata = this.files[gamePath]!;

        const isNewer = await IsNewer(metadata.sourcePath, targetPath);
        if(!isNewer)
            return;

        await EnsureDirectoryExists(path.dirname(targetPath));

        const dependencyMap = this.BuildDependencyMap(metadata.dependencies);
        if(dependencyMap.length === 0)
        {
            await fs.promises.copyFile(metadata.sourcePath, targetPath);
            await fs.promises.chmod(targetPath, 0o664);
        }
        else
        {
            const extension = path.extname(metadata.sourcePath).substring(1);
            switch(extension)
            {
                case "p3d":
                {
                    const p3dEditInput = dependencyMap.Values().ToDictionary(x => x.from, x => x.to);
                    const inputFilePath = path.join(this.jobsTempPath, pseudoRandomBytes(16).toString("hex") + ".json");
                    await fs.promises.writeFile(inputFilePath, JSON.stringify(p3dEditInput), "utf-8");

                    await CallLocalBin("p3dEdit", [metadata.sourcePath, "replace-textures", inputFilePath, ">", targetPath]);
                    await fs.promises.unlink(inputFilePath);
                }
                break;
                case "wrp":
                {
                    const idmap = (metadata.patchData === undefined) ? {} : (metadata.patchData.Values().ToDictionary( (x:any) => x.id, (x:any) => x.mappedId));
                    const wrpEditInput = {
                        ids: idmap,
                        models: dependencyMap.Values().ToDictionary(x => x.from, x => x.to)
                    };
                    const inputFilePath = path.join(this.jobsTempPath, pseudoRandomBytes(16).toString("hex") + ".json");
                    await fs.promises.writeFile(inputFilePath, JSON.stringify(wrpEditInput), "utf-8");

                    await CallLocalBin("wrpEdit", [metadata.sourcePath, "replace-resources", inputFilePath, ">", targetPath]);
                    await fs.promises.unlink(inputFilePath);
                }
                break;
                default:
                    throw new Error("impl: " + extension);
            }
        }
    }

    private MapExtensionToFolderName(extension: string)
    {
        switch(extension.toLowerCase())
        {
            case "ogg":
            case "wav":
            case "wss":
                return "sounds";
            case "paa":
            case "pac":
                return "tex";
            case "p3d":
                return "models";
            case "rtm":
                return "animations";
            case "wrp":
                return "worlds";
        }
        throw new Error("MapExtensionToFolderName not implemented: " + extension);
    }

    private async MarkForImportImpl(gamePath: string, sourcePath: string, mark: boolean)
    {
        if(gamePath in this.files)
        {
            this.files[gamePath]!.marked ||= mark;
            return;
        }

        const rawDependencies = await this.AnalyzeDependencies(sourcePath);
        const optDependencies = await rawDependencies.Values().Map(async x => await this.pboFileCatalog.ResolveCaseInsensitivePath(x)).PromiseAll();
        const dependencies = optDependencies.Values().NotUndefined().ToArray();
        this.files[gamePath] = {
            dependencies,
            marked: mark,
            sourcePath
        };

        for (const dependency of dependencies)
        {
            const sourcePath = await this.pboFileCatalog.ProvideFile(dependency);
            if(sourcePath === undefined)
                console.error("Can't find resource: " + dependency);
            else
                await this.MarkForImportImpl(dependency, sourcePath, false);
        }
    }
}