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
import { Dictionary } from "acts-util-core";
import path from "path";

export class CaseInsensitiveCacheNode
{
    constructor()
    {
        this.children = {};
    }

    //Public methods
    public AddChild(name: string, child: CaseInsensitiveCacheNode)
    {
        this.children[name.toLowerCase()] = {
            childNode: child,
            realName: name
        };
    }

    public Match(segments: string[]): string | undefined
    {
        const child = this.children[segments[0].toLowerCase()];
        if(child === undefined)
            return undefined;

        if(segments.length === 1)
            return child.realName;

        const childMatch = child.childNode.Match(segments.slice(1));
        if(childMatch === undefined)
            return undefined;

        return path.join(child.realName, childMatch);
    }

    //Private state
    private children: Dictionary<{ childNode: CaseInsensitiveCacheNode; realName: string; }>;

    //Class functions
    public static async Build(dirPath: string)
    {
        const node = new CaseInsensitiveCacheNode;

        const children = await fs.promises.readdir(dirPath, "utf-8");
        for (const childName of children)
        {
            const childPath = path.join(dirPath, childName);
            const stat = await fs.promises.stat(childPath);
            if(stat.isDirectory())
            {
                const child = await CaseInsensitiveCacheNode.Build(childPath);
                node.AddChild(childName, child);
            }
            else
                node.AddChild(childName, new CaseInsensitiveCacheNode);
        }

        return node;
    }
}