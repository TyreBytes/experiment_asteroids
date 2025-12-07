///
/// @file
/// @details This is an area for Asteroids Utility Functions...
///
/// <!-- Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved -->
///------------------------------------------------------------------------------------------------------------------///

#include "../utilities/utilities.hpp"

#include <sstream>

//--------------------------------------------------------------------------------------------------------------------//

// TODO: Asteroids: 2025-10-24: FromDynamicStructure<AnchorLocation> should be in TurtlesBrains but it requires the
//   api break and other notes here...
//
//   There should be a ToString/FromString with a Graphics Anchor in TurtleBrains that can handle this. Or even a
//   To/From DynamicStructure which should have defined method for ALL projects. We've sort of made it here and there
//   and everywhere, sometimes slightly different. This might be one of those TurtleBrains things that need to be in
//   global space, kinda like the operator<<.
//
//   When we tackle this I'd like to see if we can add a debugInfo version at the same time, such that:
//      template<typename Type> Type FromDynamicStructure(const tbCore::DynamicStructure& input);
//      template<typename Type> tbCore::DynamicStructure ToDynamicStructure(const Type& type);
//   Will automatically call the following with empty debug info?
//      template<typename Type> Type FromDynamicStructure(const tbCore::DynamicStructure& input, const String& debugInfo);
//      template<typename Type> tbCore::DynamicStructure ToDynamicStructure(const Type& type, const String& debugInfo);
//
//   I'm not exactly sure how this needs to work, but it'd sure be nice to have some way to say "this file" is causing
//   the parsing issue.

template<> Asteroids::AnchorLocation Asteroids::FromDynamicStructure(const DynamicStructure& data)
{
	const String anchorName = tbCore::StringUtilities::Lowercase(data.AsStringWithDefault(""));
	if ("topleft" == anchorName || "top_left" == anchorName) { return Anchor::TopLeft; }
	else if ("topcenter" == anchorName || "top_center" == anchorName) { return Anchor::TopCenter; }
	else if ("topright" == anchorName || "top_right" == anchorName) { return Anchor::TopRight; }
	else if ("centerleft" == anchorName || "center_left" == anchorName) { return Anchor::CenterLeft; }
	else if ("center" == anchorName) { return Anchor::Center; }
	else if ("centerright" == anchorName || "center_right" == anchorName) { return Anchor::CenterRight; }
	else if ("bottomleft" == anchorName || "bottom_left" == anchorName) { return Anchor::BottomLeft; }
	else if ("bottomcenter" == anchorName || "bottom_center" == anchorName) { return Anchor::BottomCenter; }
	else if ("bottomright" == anchorName || "bottom_right" == anchorName) { return Anchor::BottomRight; }

	tb_error("Unknown AnchorLocation specified in data.");
	return Anchor::TopLeft;
}

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::DynamicStructure Asteroids::Utilities::LoadJsonFile(const String& filepath)
{
//	return tbCore::LoadJsonFile(filepath);

	const String originalJson = tbCore::FileUtilities::LoadFileContentsToString(filepath);
	String modifiedJson = originalJson;

	std::istringstream input(originalJson);
	std::ostringstream output;
	String line;

	while (std::getline(input, line))
	{
		tbCore::StringUtilities::TrimLeadingWhitespaceInPlace(line);
		if (line.size() < 2 || line[0] != '/' || line[1] != '/')
		{
			output << line << '\n';
		}

		//const auto firstNonSpace = line.find_first_not_of(" \t");

		//if (firstNonSpace != String::npos && firstNonSpace + 1 < line.size() &&
		//	line[firstNonSpace] == '/' && line[firstNonSpace + 1] == '/')
		//{
		//	//This is a comment, (line is a comment), don't output the line.
		//}
		//else
		//{
		//	output << line << '\n';
		//}
	}

	modifiedJson = output.str();
	return tbCore::ParseJson(modifiedJson);
}

//--------------------------------------------------------------------------------------------------------------------//

namespace Asteroids::Utilities::Implementation
{
	// 2025-10-24: This does assume the array is holding structures, and that the structure will have a member "id"
	//   then find a matching element to return.
	const DynamicStructure& FindBaseInArray(const String& baseId, const DynamicStructure& basesArray)
	{
		if (false == basesArray.IsArray())
		{
			return DynamicStructure::Nil();
		}

		for (const DynamicStructure& elementData : basesArray.AsArray())
		{
			if (elementData["id"].AsString() == baseId)
			{
				return elementData;
			}
		}

		return DynamicStructure::Nil();
	}

	void CombineChildWithBaseInPlace(DynamicStructure& childData, const DynamicStructure& baseData)
	{
		//CombineChildWithBase will add any members base has that child is missing
		//  and then return the result. If child has that member it will remain
		//  untouched.

		for (auto& keyValuePair : baseData.AsStructure())
		{
			if (false == childData.HasMember(keyValuePair.first))
			{
				childData[keyValuePair.first] = keyValuePair.second;
			}
		}
	}
};

//--------------------------------------------------------------------------------------------------------------------//

Asteroids::DynamicStructure Asteroids::Utilities::CreateActualDataFromPotentialBases(const DynamicStructure& originalData,
	const String& baseMemberName, const DynamicStructure& basesArray)
{
	std::vector<DynamicStructure> bases;
	DynamicStructure actualData = originalData;

	int infiniteDetector = 10;

	DynamicStructure childData = originalData;
	tb_debug_log("Loading " << actualData["id"].AsString() << tbCore::Debug::ContinueEntry());
 	while (true == childData.HasMember(baseMemberName))
 	{
 		DynamicStructure base = Implementation::FindBaseInArray(childData[baseMemberName].AsString(), basesArray);
 		bases.push_back(base);

		tb_debug_log(":" << base["id"].AsString() << tbCore::Debug::ContinueEntry());

 		childData = base;

		--infiniteDetector;
		tb_error_if(infiniteDetector <= 0, "Expected no infinite loops with bases, '%s'", baseMemberName.c_str());
 	}

	tb_debug_log(""); //finish the entry


 	while (false == bases.empty())
 	{
 		Implementation::CombineChildWithBaseInPlace(actualData, bases.back());
		bases.pop_back();
 	}

	return actualData;
}

//--------------------------------------------------------------------------------------------------------------------//
