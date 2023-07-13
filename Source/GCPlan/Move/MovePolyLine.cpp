#include "MovePolyLine.h"

#include "Components/SceneComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"

#include "../Common/UnrealGlobal.h"
#include "../Mesh/LoadContent.h"

MovePolyLine* MovePolyLine::pinstance_{nullptr};
std::mutex MovePolyLine::mutex_;

MovePolyLine::MovePolyLine() {
}

MovePolyLine::~MovePolyLine() {
}

MovePolyLine *MovePolyLine::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new MovePolyLine();
	}
	return pinstance_;
}

void MovePolyLine::Tick(float DeltaTime) {
	FString key;
	// TArray<FString> removeKeys = {};
	float speed, newScaleXDiff, currentScaleXStart;
	int index, segementsCounter;
	FString actorName;
	AActor* actor;
	FVector scale, currentScale;
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	float maxSegments = 10;
	for (auto& Elem : _movingLines) {
		if (Elem.Value.movingActive || Elem.Value.scalingActive) {
			key = Elem.Key;
			if (Elem.Value.startTickDelay > 0) {
				_movingLines[key].startTickDelay -= 1;
			} else {
				if (Elem.Value.movingActive) {
					speed = Elem.Value.speed;
					newScaleXDiff = speed * DeltaTime;
					// In case want to do multiple segments at once.
					segementsCounter = 0;
					while (newScaleXDiff > 0) {
						currentScaleXStart = _movingLines[key].currentScaleX;
						_movingLines[key].currentScaleX += newScaleXDiff;
						// UE_LOG(LogTemp, Display, TEXT("currentIndex %d scale %f maxScale %f"), Elem.Value.currentIndex, _movingLines[key].currentScaleX, Elem.Value.currentMaxScaleX);
						if (_movingLines[key].currentScaleX > _movingLines[key].currentMaxScaleX) {
							newScaleXDiff -= (_movingLines[key].currentMaxScaleX - currentScaleXStart);
							_movingLines[key].currentScaleX = _movingLines[key].currentMaxScaleX;
						} else {
							newScaleXDiff = 0;
						}

						SetScale(key);

						if (_movingLines[key].currentScaleX >= _movingLines[key].currentMaxScaleX) {
							_movingLines[key].currentIndex += 1;
							_movingLines[key].currentScaleX = 0;
							if (_movingLines[key].currentIndex >= (Elem.Value.vertices.Num() - 1)) {
								// removeKeys.Add(key);
								_movingLines[key].movingActive = false;
							} else {
								index = _movingLines[key].currentIndex;
								_movingLines[key].currentMaxScaleX = GetLineMaxXScale(Elem.Value.vertices[(index)],
									Elem.Value.vertices[(index + 1)]);
							}
						}
						segementsCounter += 1;
						// UE_LOG(LogTemp, Display, TEXT("MPL.Tick segementsCounter %d key %s newScaleXDiff %f"), segementsCounter, *key, newScaleXDiff);
						if (segementsCounter > maxSegments) {
							break;
						}
					}
				}

				if (Elem.Value.scalingActive) {
					if (Elem.Value.maxScale >= 0 && Elem.Value.scale.Y > Elem.Value.maxScale) {
						_movingLines[key].scale.Y -= Elem.Value.scaleSpeed * DeltaTime;
						_movingLines[key].scale.Z -= Elem.Value.scaleSpeed * DeltaTime;
						// UE_LOG(LogTemp, Display, TEXT("key %s new scale %f"), *key, _movingLines[key].scale.Y);
						if (_movingLines[key].scale.Y < Elem.Value.maxScale) {
							_movingLines[key].scale.Y = Elem.Value.maxScale;
						}
						if (_movingLines[key].scale.Z < Elem.Value.maxScale) {
							_movingLines[key].scale.Z = Elem.Value.maxScale;
						}
						// Need to update ALL line segments.
						for (int ss = 0; ss < Elem.Value.vertices.Num(); ss++) {
							actorName = FormName(key, ss);
							if (_actors.Contains(actorName)) {
								actor = _actors[actorName];
								currentScale = actor->GetActorScale();
								if (currentScale.X > 0) {
									scale = FVector(currentScale.X, _movingLines[key].scale.Y, _movingLines[key].scale.Z);
									actor->SetActorScale3D(scale);
									// Need to reset location?? Scale changes it??
									actor->SetActorLocation(Elem.Value.vertices[ss] * unrealGlobal->GetScale());
								}
							}
						}
					} else {
						_movingLines[key].scalingActive = false;
						// UE_LOG(LogTemp, Display, TEXT("key %s scaling done max %f current %f"), *key, Elem.Value.maxScale, Elem.Value.scale.Y);
					}
				}
			}
		}
	}
	// for (int ii = 0; ii < removeKeys.Num(); ii++) {
	// 	Remove(removeKeys[ii]);
	// }
}

void MovePolyLine::SetScale(FString key) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	FString actorName = FormName(key, _movingLines[key].currentIndex);
	if (_actors.Contains(actorName)) {
		AActor* actor = _actors[actorName];
		FVector scale = FVector(_movingLines[key].currentScaleX, _movingLines[key].scale.Y, _movingLines[key].scale.Z);
		actor->SetActorScale3D(scale);
		// Need to reset location?? Scale changes it??
		actor->SetActorLocation(_movingLines[key].vertices[_movingLines[key].currentIndex] * unrealGlobal->GetScale());
	}
}

void MovePolyLine::CleanUp(bool destroy) {
	if (destroy) {
		UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
		for (auto& Elem : _actors) {
			if (IsValid(Elem.Value)) {
				unrealGlobal->RemoveAttachedActors(Elem.Value);
				Elem.Value->Destroy();
			} else {
				UE_LOG(LogTemp, Display, TEXT("NOT destroying %s"), *Elem.Key);
			}
		}
	}
	_actors.Empty();

	_movingLines.Empty();
}

void MovePolyLine::Remove(FString name) {
	if (_movingLines.Contains(name)) {
		_movingLines.Remove(name);
		// Remove / destroy actor too?
		// TODO
	}
}

void MovePolyLine::SetWorld(UWorld* world) {
	_world = world;
}

void MovePolyLine::ReScale(float maxScale, float scaleSpeed) {
	for (auto& Elem : _movingLines) {
		_movingLines[Elem.Key].maxScale = maxScale;
		_movingLines[Elem.Key].scaleSpeed = scaleSpeed;
		_movingLines[Elem.Key].scalingActive = true;
		// UE_LOG(LogTemp, Display, TEXT("ReScale key %s scale %f maxScale %f"), *Elem.Key, _movingLines[Elem.Key].scale.Y, _movingLines[Elem.Key].maxScale);
	}
}

float MovePolyLine::GetLineMaxXScale(FVector start, FVector end) {
	// Assumes base scale of 1 = 1 meter, so max scale is distance in meters.
	float distance = (end - start).Size();
	return distance;
}

FString MovePolyLine::FormName(FString key, int ii) {
	return key + "_" + FString::FromInt(ii);
}

void MovePolyLine::CreateActors(FString key, TArray<FVector> vertices, AActor* actorTemplate,
	FString materialKey, FVector scaleBase) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	LoadContent* loadContent = LoadContent::GetInstance();
	FString name;
	AStaticMeshActor* actor;
	FActorSpawnParameters spawnParams;
	spawnParams.Template = actorTemplate;
	UStaticMeshComponent* meshComponent;
	UMaterialInterface* material = nullptr;
	FString materialPath = loadContent->Material(materialKey);
	FRotator rotator;
	FVector diff;
	if (materialPath.Len() > 0) {
		if (materialPath.Contains(".MaterialInstance")) {
			material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL,
				*materialPath));
		} else {
			material = Cast<UMaterialInterface>(StaticLoadObject(UMaterial::StaticClass(), NULL,
				*materialPath));
		}
	}

	// Do not leed the last vertex since no line after it.
	for (int ii = 0; ii < vertices.Num() - 1; ii++) {
		name = FormName(key, ii);
		spawnParams.Name = FName(name);
		// Rotate toward next vector
		// https://forums.unrealengine.com/t/easy-question-get-a-rotation-by-2-vectors/330006
		diff = (vertices[ii + 1] - vertices[ii]);
		diff.Normalize();
		rotator = diff.Rotation();
		actor = (AStaticMeshActor*)_world->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(), vertices[ii] * unrealGlobal->GetScale(), rotator, spawnParams);
		// Need to set movable too for location reseting when scale..
		USceneComponent* component = actor->FindComponentByClass<USceneComponent>();
		component->SetMobility(EComponentMobility::Movable);
		_actors.Add(name, actor);
		actor->SetActorLabel(name);
		actor->SetActorScale3D(FVector(0,0,0));
		// Need to reset location after rotation, since rotation (and scale?) will change location..
		actor->SetActorLocation(vertices[ii] * unrealGlobal->GetScale());
		if (material) {
			meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
			meshComponent->SetMaterial(0, material);
		}
	}
}

void MovePolyLine::Move(FString key, TArray<FVector> vertices, AActor* actorTemplate,
	FString materialKey, float speed, FVector scale, int startTickDelay) {
	CreateActors(key, vertices, actorTemplate, materialKey, scale);
	float maxScaleX = GetLineMaxXScale(vertices[0], vertices[1]);
	FMovePolyLine moving = FMovePolyLine(key, vertices, actorTemplate, speed, scale,
		startTickDelay, materialKey, maxScaleX);
	if (_movingLines.Contains(key)) {
		_movingLines[key] = moving;
	} else {
		_movingLines.Add(key, moving);
	}
}

void MovePolyLine::DrawFull(FString key, TArray<FVector> vertices, AActor* actorTemplate,
	FString materialKey) {
	CreateActors(key, vertices, actorTemplate, materialKey, FVector(1,1,1));
	AActor* actor;
	FString actorName;
	float maxScaleX;
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	for (int ii = 0; ii < vertices.Num() - 1; ii++) {
		maxScaleX = GetLineMaxXScale(vertices[ii], vertices[ii+1]);
		actorName = FormName(key, ii);
		if (_actors.Contains(actorName)) {
			actor = _actors[actorName];
			actor->SetActorScale3D(FVector(maxScaleX,1,1));
			// Need to reset location?? Scale changes it??
			actor->SetActorLocation(vertices[ii] * unrealGlobal->GetScale());
		}
	}
}
