// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperSurfaceFinishData.h"

FGameplayTag UFlipperSurfaceFinishData::ResolveAppliedFinish(const FGameplayTag& PaintFinish, const FGameplayTag& TargetFinish) const
{
	// the paint is exactly what the surface wants, e.g. white paint on a wall that wants white paint
	if (PaintFinish == TargetFinish) return TargetFinish;
	
	// the surface wants a finish that this paint produces, e.g. red paint on a wall that wants red brick
	const FFlipperSurfaceFinish* TargetSurfaceFinish = GetFinish(TargetFinish);
	if (TargetSurfaceFinish && TargetSurfaceFinish->AppliedByPaint == PaintFinish) return TargetFinish;
	
	// wrong paint for this surface, so the paint is applied as is, tinting the existing texture
	return PaintFinish;
}
