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

export interface CompileConfigStep
{
    type: "CompileConfig";
    source: string;
    sourceFile: string;
    targetFolder: string;
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

export interface ImportFilesStep
{
    type: "ImportFiles";
    source: string;
    sources: {
        type: "Archive";
        name: string;
        pbos: string[];
        files: string[];
    }[];
}

export interface PackArchiveStep
{
    type: "PackArchive";
    source: string;
    source_folder: string;
    target_folder: string;
}

export interface RepackArchiveStep
{
    type: "RepackArchive";
    sourceLocation: string;
    exclude: string[];
    targetPboName: string;

    include: {
        source: string;
        path: string;
    }[];

    source: {
        type: "Archive";
        name: string;
        pbo: string;
    };
}

export type PipelineStep = CompileConfigStep | CopyFilesStep | Create7zArchiveStep | ImportFilesStep | PackArchiveStep | RepackArchiveStep;

export interface PipelineDefinition
{
    steps: PipelineStep[];
}