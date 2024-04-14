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

import { Dictionary } from "acts-util-core";

interface ArchiveLocation
{
    type: "Archive";
    sourceLocation: string;
    name: string;
}

interface FileSystemLocation
{
    type: "FileSystem";
    sourceLocation: string;
}
type Location = ArchiveLocation | FileSystemLocation;

type PBOLocation = Location & { pbo: string; };

interface ArchiveWithPBOsFileSource extends ArchiveLocation
{
    pbos: string[];
}

interface ArchiveWithoutPBOsFileSource
{
    type: "FlatArchive";
    sourceLocation: string;
    name: string;
}

interface FileSystemFileSource extends FileSystemLocation
{
    files: string | string[];
}

interface PBOOnFileSystemSource
{
    type: "FileSystemPBO";
    sourceLocation: string;
    pbo: string;
}

type FileSource = ArchiveWithPBOsFileSource | ArchiveWithoutPBOsFileSource | FileSystemFileSource | PBOOnFileSystemSource;
type FileSourceWithFiles<T> = FileSource & { files: T | T[]; };


interface DtaExtStepFileEntry
{
    sourceFileName: string;
    type: "base" | "magazine" | "weapon";
    targetName: string;
}

interface RepackFileEntry
{
    sourceFileName: string;
    targetFileName: string;
}

export interface BuildDtaExtStep
{
    type: "BuildDtaExt";
    sourceLocation: string;
    id_map: {
        source: string;
        filePath: string;
        regExp: string;
    };
    imageSources: (ArchiveWithPBOsFileSource & { files: DtaExtStepFileEntry[]; })[];
}

export interface CompileConfigStep
{
    type: "CompileConfig";
    source: string;
    sourceFile: string;
    targetFolder: string;
    definitions?: string[];
}

export interface CopyFilesStep
{
    type: "CopyFiles";
    source: string;
    files: string | string[];
}

interface Create7zArchiveStep
{
    type: "Create7zArchive";
}

export interface ExtractPBOStep
{
    type: "ExtractPBO";
    source: PBOLocation;
    targetFolder: string;
}

type ImportFilesFileDefinition = string | { sourceFileName: string; targetName: string; patch?: any };

export interface ImportFilesStep
{
    type: "ImportFiles";
    ignore: string[];
    sources: FileSourceWithFiles<ImportFilesFileDefinition>[];
}

export interface IncludePipelineStepsStep
{
    type: "IncludePipelineSteps";
    name: string;
}

export interface PackArchiveStep
{
    type: "PackArchive";
    source: string;
    source_folder: string;
    target_folder: string;
    targetFileName?: string;
}

export interface RepackArchiveStep
{
    type: "RepackArchive";
    source: PBOLocation;
    exclude: string[];
    include: FileSourceWithFiles<string | RepackFileEntry>[];
    targetPboName: string;
}

export type PipelineStep = BuildDtaExtStep | CompileConfigStep | CopyFilesStep | Create7zArchiveStep | ExtractPBOStep | ImportFilesStep | IncludePipelineStepsStep | PackArchiveStep | RepackArchiveStep;

export interface PipelineDefinition
{
    source_integrity: Dictionary<string>;
    steps: PipelineStep[];
}