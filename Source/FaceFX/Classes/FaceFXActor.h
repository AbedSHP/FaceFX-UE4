/*******************************************************************************
  The MIT License (MIT)
  Copyright (c) 2015 OC3 Entertainment, Inc.
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*******************************************************************************/

#pragma once

#include "FaceFXConfig.h"
#include "FaceFXAsset.h"
#include "FaceFXData.h"
#include "FaceFXActor.generated.h"

/** The data that holds the FaceFX related data without the animations. The binary data in here was generated by the FaceFX compiler for all target platforms and get written into PlatformData. 
 During cooking we remove all unnecessary data and move all into the first index location */

/** Asset that can be assigned to FaceFXComponents and which contain the FaceFX runtime data */
UCLASS(hideCategories=Object)
class FACEFX_API UFaceFXActor : public UFaceFXAsset
{
	GENERATED_UCLASS_BODY()

public:

	//UObject
	virtual SIZE_T GetResourceSize(EResourceSizeMode::Type Mode) override;
	//~UObject

	/**
	* Checks if this FaceFX data asset it valid
	* @returns True if valid, else false
	*/
	virtual bool IsValid() const override
	{
		return Super::IsValid() && PlatformData.Num() > 0;
	}

#if WITH_EDITORONLY_DATA

	friend struct FFaceFXEditorTools;

	//UObject
	virtual void Serialize(FArchive& Ar) override;
	//~UObject

	/**
	* Gets the details in a human readable string representation
	* @param OutDetails The resulting details string
	*/
	virtual void GetDetails(FString& OutDetails) const override;

	/** Resets the asset */
	inline void Reset()
	{
		PlatformData.Empty();
	}

#endif //WITH_EDITORONLY_DATA

#if WITH_EDITORONLY_DATA && FACEFX_USEANIMATIONLINKAGE
	/**
	* Links this set to the given animation set
	* @param AnimSet The asset to link with
	*/
	inline void LinkTo(class UFaceFXAnim* AnimSet)
	{
		Animations.AddUnique(AnimSet);
	}

	/**
	* Unlinks this set from the given animation set
	* @param AnimSet The asset to unlink from
	* @returns True if any link was removed, else false
	*/
	inline bool UnlinkFrom(class UFaceFXAnim* AnimSet)
	{
		return Animations.Remove(AnimSet) > 0;
	}

	/**
	* Gets the number of animations which are encapsulated in this asset
	* @return The animation count
	*/
	virtual int32 GetAnimationCount() const override;

#endif //WITH_EDITOR && FACEFX_USEANIMATIONLINKAGE

	/**
	* Gets the FaceFX data for the current target platform
	* @returns The data for the current target platform
	*/
	inline FFaceFXActorData& GetData()
	{
#if WITH_EDITORONLY_DATA
		//non-cooked build - this will always be PC
		FFaceFXActorData* DataForPC = PlatformData.FindByKey(EFaceFXTargetPlatform::PC);
		checkf(DataForPC, TEXT("Asset not initialized for PC."));
		return *DataForPC;
#else		
		//cooked build - this will always be the data from the target platform during cooking
		checkf(PlatformData.Num(), TEXT("Asset not initialized yet."));
		return PlatformData[0];
#endif //WITH_EDITORONLY_DATA
	}

	/**
	* Gets the FaceFX data for the current target platform
	* @returns The data for the current target platform
	*/
	inline const FFaceFXActorData& GetData() const
	{
#if WITH_EDITORONLY_DATA
		//non-cooked build - this will always be PC
		const FFaceFXActorData* DataForPC = PlatformData.FindByKey(EFaceFXTargetPlatform::PC);
		checkf(DataForPC, TEXT("Asset not initialized for PC."));
		return *DataForPC;
#else		
		//cooked build - this will always be the data from the target platform during cooking
		checkf(PlatformData.Num(), TEXT("Asset not initialized yet."));
		return PlatformData[0];
#endif //WITH_EDITORONLY_DATA
	}

#if FACEFX_USEANIMATIONLINKAGE
	/**
	* Gets a specific animation data entry from the set
	* @param AnimGroup the animation group to look for
	* @param AnimName The animation to look for
	* @returns The animation asset if found, else nullptr
	*/
	const class UFaceFXAnim* GetAnimation(const FName& AnimGroup, const FName& AnimName) const;

	/**
	* Gets a specific animation data entry from the set
	* @param AnimId the animation id to look for
	* @returns The animation asset if found, else nullptr
	*/
	inline const class UFaceFXAnim* GetAnimation(const FFaceFXAnimId& AnimId) const
	{
		return GetAnimation(AnimId.Group, AnimId.Name);
	}

	/**
	* Gets all animation groups
	* @param OutGroups The animation groups
	*/
	void GetAnimationGroups(TArray<FName>& OutGroups) const;

	/**
	* Gets all animation ids
	* @param OutAnimIds The animation ids
	*/
	void GetAnimationIds(TArray<FFaceFXAnimId>& OutAnimIds) const;

#endif //FACEFX_USEANIMATIONLINKAGE

#if WITH_EDITORONLY_DATA
	
	/** 
	* Gets the platform data for the given target platform
	* @returns The data entry or nullptr if not found
	*/
	inline FFaceFXActorData* GetPlatformData(EFaceFXTargetPlatform::Type Platform = EFaceFXTargetPlatform::PC)
	{
		return PlatformData.FindByKey(Platform);
	}

	/** 
	* Gets the platform data for the given target platform or creates a new entry if missing
	* @returns The data entry or nullptr if not found
	*/
	inline FFaceFXActorData& GetOrCreatePlatformData(EFaceFXTargetPlatform::Type Platform = EFaceFXTargetPlatform::PC)
	{
		if(FFaceFXActorData* ExistingEntry = PlatformData.FindByKey(Platform))
		{
			return *ExistingEntry;
		}
		return PlatformData[PlatformData.Add(FFaceFXActorData(Platform))];
	}

#endif //WITH_EDITORONLY_DATA

private:

	/** The data inside this data set. Its a list of data per platform. Will only contain one entry in cooked data */
	UPROPERTY(EditInstanceOnly, Category=FaceFX)
	TArray<FFaceFXActorData> PlatformData;

	/** The linked animations where this set look up the animations in */
	UPROPERTY(EditInstanceOnly, Category=FaceFX)
	TArray<class UFaceFXAnim*> Animations;
};
