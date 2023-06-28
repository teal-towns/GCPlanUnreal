#include "Lodash.h"

#include <random>
#include "math.h"

Lodash::Lodash() {
	if (!seeded) {
		srand(time(NULL));
		seeded = true;
	}
}

Lodash::~Lodash() {
}

// public void ShuffleList<T>(List<T> list) {
// 	int n = list.Count;
// 	while (n > 1) {
// 		n--;
// 		int k = rng.Next(n + 1);
// 		T value = list[k];
// 		list[k] = list[n];
// 		list[n] = value;
// 	}
// }

FString Lodash::GetInstanceId(FString prefix, int numChars) {
	FString prefixFinal = prefix.Len() > 0 ? prefix : "";
	FString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	int charsLen = chars.Len();
	FString str = "";
	for (int ii = 0; ii < numChars; ii++) {
		str += chars[RandomRangeInt(0, charsLen - 1)];
	}
	return prefixFinal + str;
}

// public bool ListsEqual(List<string> list1, List<string> list2) {
// 	if (list1.Count != list2.Count) {
// 		return false;
// 	}
// 	for (int ii = 0; ii < list1.Count; ii++) {
// 		if (list1[ii] != list2[ii]) {
// 			return false;
// 		}
// 	}
// 	return true;
// }

// public List<string> Omit(List<string> list1, List<string> omitList, bool makeCopy = false) {
// 	if (omitList.Count < 1) {
// 		return list1;
// 	}
// 	int ii;
// 	List<string> list = new List<string>() {};
// 	if (makeCopy) {
// 		for (ii = 0; ii < list1.Count; ii++) {
// 			list.Add(list1[ii]);
// 		}
// 	} else {
// 		list = list1;
// 	}
// 	for (ii = (list.Count - 1); ii >= 0; ii--) {
// 		if (omitList.Contains(list[ii])) {
// 			list.RemoveAt(ii);
// 		}
// 	}
// 	return list;
// }

// public List<string> Pick(List<string> list1, List<string> pickList, bool makeCopy = false) {
// 	if (pickList.Count < 1) {
// 		return list1;
// 	}
// 	int ii;
// 	List<string> list = new List<string>() {};
// 	if (makeCopy) {
// 		for (ii = 0; ii < list1.Count; ii++) {
// 			list.Add(list1[ii]);
// 		}
// 	} else {
// 		list = list1;
// 	}
// 	for (ii = (list.Count - 1); ii >= 0; ii--) {
// 		if (!pickList.Contains(list[ii])) {
// 			list.RemoveAt(ii);
// 		}
// 	}
// 	return list;
// }

// // public T ChooseRandom(List<T> items) {
// //	 int index = UnityEngine.Random.Range(0, items.Count);
// //	 return items[index];
// // }

// public float ChooseRandomF(List<float> items) {
// 	int index = UnityEngine.Random.Range(0, items.Count);
// 	return items[index];
// }

int Lodash::RandomRangeInt(int min, int max) {
	return min + rand() % (( max + 1 ) - min);
}

float Lodash::RandomRangeFloat(float min, float max) {
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

float Lodash::RangeValue(float value, float valueMin, float valueMax, float newStart, float newEnd) {
	if (value <= valueMin) {
		return newStart;
	} else if (value >= valueMax) {
		return newEnd;
	}
	float changeRatio = abs((value - valueMin) / (valueMax - valueMin));
	return newStart + (newEnd - newStart) * changeRatio;
}

FString Lodash::ToFixed(float value, int digits) {
	// https://nerivec.github.io/old-ue4-wiki/pages/float-as-string-with-precision.html
	float Rounded = roundf(value);
	if (abs(value - Rounded) < pow(10, -1 * digits)) {
		value = Rounded;
	}
	FNumberFormattingOptions NumberFormat;
	// NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	// NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = digits;
	NumberFormat.MaximumFractionalDigits = digits;
	NumberFormat.UseGrouping = false;
	return FText::AsNumber(value, &NumberFormat).ToString();
}

// T Lodash::Max(TArray<T> values) {
// 	T val = values[0];
// 	for (int ii = 0; ii < values.Num(); ii++) {
// 		if (values[ii] > val) {
// 			val = values[ii];
// 		}
// 	}
// 	return val;
// }

// T Lodash::Min(TArray<T> values) {
// 	T val = values[0];
// 	for (int ii = 0; ii < values.Num(); ii++) {
// 		if (values[ii] < val) {
// 			val = values[ii];
// 		}
// 	}
// 	return val;
// }

// TODO - allow any type.
// template <typename T> TMap<FString, T> Lodash::Object(TMap<FString, T> values, TMap<FString, T> defaults) {
TMap<FString, float> Lodash::Object(TMap<FString, float> values, TMap<FString, float> defaults) {
	for (auto& Elem : defaults) {
		if (!values.Contains(Elem.Key)) {
			values.Add(Elem.Key, Elem.Value);
		}
	}
	return values;
}

TMap<FString, FString> Lodash::PairsStringToObject(FString pairsString) {
	TMap<FString, FString> keyVals;
	if (pairsString.Len() < 1) {
		return keyVals;
	}
	TArray<FString> keyValsArray;
	pairsString.ParseIntoArray(keyValsArray, TEXT("&"), true);
	TArray<FString> tempArray;
	for (int ii = 0; ii < keyValsArray.Num(); ii++) {
		keyValsArray[ii].ParseIntoArray(tempArray, TEXT("="), true);
		if (tempArray.Num() == 2) {
			keyVals.Add(tempArray[0], tempArray[1]);
		} else if (tempArray.Num() == 1) {
			keyVals.Add(tempArray[0], "1");
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Lodash.PairsStringToObject no length, skipping"));
		}
	}
	return keyVals;
}

FString Lodash::CheckAddSuffix(FString text, FString suffix) {
	int len = suffix.Len();
	if (text.Len() > 1 && !(text.RightChop((text.Len() - len)) == suffix)) {
		text += suffix;
	}
	return text;
}

FString Lodash::Join(TArray<FString> list, FString delimiter) {
	FString text;
	for (int ii = 0; ii < list.Num(); ii++) {
		text += list[ii];
		if (ii < list.Num() - 1) {
		 	text += delimiter;
		}
	}
	return text;
}
