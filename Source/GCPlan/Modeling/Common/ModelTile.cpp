#include "ModelTile.h"

#include "Engine/StaticMeshActor.h"

//#include "../ModelBase.h"
//#include "../../Mesh/LoadContent.h"
//#include "../../ModelingStructsActor.h"
//#include "../../ProceduralModel/PMCylinder.h"
#include "Math/UnrealMathUtility.h"

TArray<FString> tileTypes = {
	"black",// Index 0
	"white",// Index 1
	"grey",	// Index 2
	"blue"	// Index 3
};			// Max = 4
#define MAXTILETYPE	4	// black and blue seem to be the same color
#define MAXTILEINDX (MAXTILETYPE - 1)
#define DEFTILESIZE	0.1
#define MINTILESNUM	4	// minimum Number of Tiles  4 x  4 = 16
#define MAXTILESNUM	64	// maximum Number of Tiles 64 x 64 = 4096

ModelTile::ModelTile() {
}

ModelTile::~ModelTile() {
}

void ModelTile::Left(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams)
{
	FVector tile = FVector(scale.X, 0, scale.Z);
	doTile(actor, name, location, rotation, scale, spawnParams, modelParams, tile);
}

void ModelTile::Right(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams)
{
	FVector tile = FVector(scale.X, 0, scale.Z);
	doTile(actor, name, location, rotation, scale, spawnParams, modelParams, tile);
}

void ModelTile::Back(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams)
{
	FVector tile = FVector(0, scale.Y, scale.Z);
	doTile(actor, name, location, rotation, scale, spawnParams, modelParams, tile);
}

void ModelTile::Front(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams)
{
	FVector tile = FVector(0, scale.Y, scale.Z);
	doTile(actor, name, location, rotation, scale, spawnParams, modelParams, tile);
}

void ModelTile::Bottom(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams)
{
	FVector tile = FVector(scale.X, scale.Y, 0);
	doTile(actor, name, location, rotation, scale, spawnParams, modelParams, tile);
}

void ModelTile::Top(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams)
{
	FVector tile = FVector(scale.X, scale.Y, 0);
	doTile(actor, name, location, rotation, scale, spawnParams, modelParams, tile);
}

void ModelTile::doTile(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
			FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams, FVector tile)
{
	int		xNum = 0, yNum = 0, zNum = 0;
	int		xKnt = 0, yKnt = 0, zKnt = 0;
	int		maxTiles = 1;
	float	xOrg, yOrg, zOrg, xxx, yyy, zzz;
	float	xTileScale = scale.X, yTileScale = scale.Y, zTileScale = scale.Z;

	modelParams.meshKey = "cube";

	xNum = (int)((tile.X / DEFTILESIZE) + 0.5);
	if (xNum > 0) {
		if (xNum < MINTILESNUM) xNum = MINTILESNUM;
		if (xNum > MAXTILESNUM) xNum = MAXTILESNUM;
		maxTiles *= xNum;
		xTileScale = tile.X / xNum;
	}
	UE_LOG(LogTemp, Display, TEXT("doTile tile.X %f xNum %d xTileScale %f maxTiles %d"),
									tile.X, xNum, xTileScale, maxTiles);//smmdebug230627

	yNum = (int)((tile.Y / DEFTILESIZE) + 0.5);
	if (yNum > 0) {
		if (yNum < MINTILESNUM) yNum = MINTILESNUM;
		if (yNum > MAXTILESNUM) yNum = MAXTILESNUM;
		maxTiles *= yNum;
		yTileScale = tile.Y / yNum;
	}
	UE_LOG(LogTemp, Display, TEXT("doTile tile.Y %f yNum %d yTileScale %f maxTiles %d"),
									tile.Y, yNum, yTileScale, maxTiles);//smmdebug230627

	zNum = (int)((tile.Z / DEFTILESIZE) + 0.5);
	if (zNum > 0) {
		if (zNum < MINTILESNUM) zNum = MINTILESNUM;
		if (zNum > MAXTILESNUM) zNum = MAXTILESNUM;
		maxTiles *= zNum;
		zTileScale = tile.Z / zNum;
	}
	UE_LOG(LogTemp, Display, TEXT("doTile tile.Z %f zNum %d zTileScale %f maxTiles %d"),
									tile.Z, zNum, zTileScale, maxTiles);//smmdebug230627

	FVector tileScale = FVector(xTileScale, yTileScale, zTileScale);
	xOrg = location.X + (xTileScale / 2.0);
	yOrg = location.Y + (yTileScale / 2.0);
	zOrg = location.Z + (zTileScale / 2.0);
	xxx = xOrg;
	yyy = yOrg;
	zzz = zOrg;
	ModelBase* modelBase = ModelBase::GetInstance();
	if (modelBase) {
		for (int ii = 0; ii < maxTiles; ii++) {
			int iType = FMath::RandRange(0, MAXTILEINDX);
			modelParams.materialKey = tileTypes[iType];
			FVector tileLoc = FVector(xxx, yyy, zzz);
			FString fsix = FString::FromInt(ii);
			modelBase->CreateActor(name + fsix, tileLoc, rotation, tileScale, spawnParams, modelParams);
			if (xNum > 0) {
				xKnt++;
				xxx += xTileScale;
				if (xKnt >= xNum) {
					xxx = xOrg;
					xKnt = 0;
				}
			}
			if (yNum > 0 && xKnt == 0) {
				yyy += yTileScale;
				yKnt++;
				if (yKnt >= yNum) {
					yyy = yOrg;
					yKnt = 0;
				}
			}
			if (zNum > 0 && xKnt == 0 && yKnt == 0) {
				zzz += zTileScale;
				zKnt++;
				if (zKnt >= zNum) {
					zzz = zOrg;
					zKnt = 0;
				}
			}
		}
		UE_LOG(LogTemp, Display, TEXT("doTile loc.X %f xxx %f loc.X %f xxx %f loc.Z %f zzz %f"),
							location.X, xxx, location.Y, yyy, location.Z, zzz);//smmdebug230627
	}
}
