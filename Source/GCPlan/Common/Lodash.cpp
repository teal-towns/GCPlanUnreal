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
	FString prefixFinal = prefix.Len() > 0 ? prefix + "_" : "";
	FString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	int charsLen = chars.Len();
	FString str = "";
	for (int ii = 0; ii < numChars; ii++) {
		str += chars[RandomRangeInt(0, charsLen - 1)];
	}
	return prefixFinal + "_" + str;
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
	return FText::AsNumber(value, &NumberFormat).ToString();
}
