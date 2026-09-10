#include <windows.h>
#include <stdlib.h>


// Экспортируемая функция: удаляет пробелы из inputPath, пишет в outputPath
// Возвращает: количество удалённых пробелов (>=0) или код ошибки (<0)
__declspec(dllexport) int ProcessFile(const char* inputPath, const char* outputPath) 
{
	// 1. Открываем входной файл
	HANDLE hin = CreateFileA(inputPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hin == INVALID_HANDLE_VALUE) return -1;

	// 2. Узнаём размер
	DWORD size = GetFileSize(hin, NULL);
	if (size == INVALID_FILE_SIZE) { CloseHandle(hin); return -2; }

	// 3. Отобразить файл в память
	HANDLE hmap = CreateFileMappingA(hin, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hmap) { CloseHandle(hin); return -3; }

	char* src = (char*)MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 0);
	if (!src) { CloseHandle(hmap); CloseHandle(hin); return -4; }

	// 4. Обраотка файла, удаление пробелов
	char* dst = (char*)malloc(size);
	if (!dst) { UnmapViewOfFile(src); CloseHandle(hmap); CloseHandle(hin); return -5; }

	int removed = 0, pos = 0;
	for (DWORD i = 0; i < size; i++) 
	{
		if (src[i] == ' ') removed++;
		else dst[pos++] = src[i];
	}

	UnmapViewOfFile(src);
	CloseHandle(hmap);
	CloseHandle(hin);

	// 5. Записать результат в выходной файл
	HANDLE hout = CreateFileA(outputPath, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hout == INVALID_HANDLE_VALUE) { free(dst); return -6; }

	DWORD written;
	WriteFile(hout, dst, pos, &written, NULL);
	free(dst);
	CloseHandle(hout);

	return removed;  // успех — сколько пробелов убрали
}