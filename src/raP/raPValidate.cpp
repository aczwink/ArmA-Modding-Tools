/*
 * Copyright (c) 2023 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of ArmA-Modding-Tools.
 *
 * ArmA-Modding-Tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArmA-Modding-Tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ArmA-Modding-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */
//Class header
#include <libBISMod/raP/raP.hpp>
#include <libBISMod/raP/raPValidationException.hpp>
//Namespaces
using namespace libBISMod;
using namespace StdXX;

class ScopeLookup
{
public:
	//Methods
	void DefineClass(const RapNode& classNode)
	{
		if(classNode.IsDerived())
		{
			const RapNode* parent = this->ResolveParentClass(classNode);
			this->derivationSources[&classNode] = parent;
		}

		auto key = classNode.GlobalName().ToLowercase();
		if(this->globalClassLookup.Contains(key))
			throw raPValidationException(u8"Class '" + classNode.GetName() + u8"' exists twice at the same scope: " + classNode.GlobalName());
		this->globalClassLookup.Insert(key, &classNode);
	}

private:
	//State
	BinaryTreeMap<String, const RapNode*> globalClassLookup;
	BinaryTreeMap<const RapNode*, const RapNode*> derivationSources;

	//Methods
	const RapNode* ResolveParentClass(const RapNode& classNode)
	{
		auto classLookupName = classNode.InheritedClassName().ToLowercase();
		const RapNode* searchNode = &classNode;
		while(searchNode->Parent())
		{
			auto baseClass = this->TryResolveBaseClassFromSibling(classLookupName, *searchNode);
			if(baseClass)
				return baseClass;

			baseClass = this->TryResolveBaseClassFromDerivationChain(classLookupName, *searchNode->Parent());
			if(baseClass)
				return baseClass;

			searchNode = searchNode->Parent();
		}

		throw raPValidationException(u8"Unknown base class '" + classNode.InheritedClassName() + u8"' of class: " + classNode.GlobalName());
	}

	const RapNode* TryResolveBaseClassFromDerivationChain(const String &classLookupName, const RapNode &searchNode)
	{
		if(searchNode.IsDerived())
		{
			auto it = this->derivationSources.Find(&searchNode);
			if(it == this->derivationSources.end())
				return nullptr;
			auto node = this->TryResolveParentClassFromChildren(classLookupName, *(*it).value);
			if(node)
				return node;
			return this->TryResolveBaseClassFromDerivationChain(classLookupName, *(*it).value);
		}
		return nullptr;
	}

	const RapNode* TryResolveParentClassFromChildren(const String& classLookupName, const RapNode& classNode)
	{
		for(const auto& child : classNode.ChildNodes())
		{
			if(child->GetName().ToLowercase() == classLookupName)
				return child.operator->();
		}
		return nullptr;
	}

	const RapNode* TryResolveBaseClassFromSibling(const String &classLookupName, const RapNode &classNode)
	{
		for(const auto& child : classNode.Parent()->ChildNodes())
		{
			if(child.operator->() == &classNode)
				break;

			if(child->GetName().ToLowercase() == classLookupName)
				return child.operator->();
		}
		return nullptr;
	}

	/*
	//Methods
	const RapNode* LookupClass(const RapNode& referenceNode, const String& className)
	{
		auto it = this->lookup.Find(referenceNode.GlobalName().ToLowercase());
		if(it == this->lookup.end())
			return nullptr;

		const RapNode* current = (*it).value;
		while(current)
		{
			if(current->GetName() == className)
				return current;
			current = this->prevMap.Get(current);
		}

		return current;
	}

	//Inline
	inline void Insert(const RapNode& node, const RapNode* prev)
	{
		this->prevMap[&node] = prev;
	}
	*/
};

//Local functions
static void ValidateNode(const RapNode& node, ScopeLookup& scopes)
{
	switch(node.PacketType())
	{
		case RAP_PACKETTYPE_CLASS:
		{
			scopes.DefineClass(node);
			for(const auto& childNode : node.ChildNodes())
				ValidateNode(*childNode, scopes);
		}
		break;
		case RAP_PACKETTYPE_VARIABLE:
			break;
		case RAP_PACKETTYPE_ARRAY:
			break;
	}
}

//Namespaces functions
void libBISMod::ValidateRapTree(const RapTree& tree)
{
	ScopeLookup scopes;
	ValidateNode(*tree.rootNode, scopes);
}