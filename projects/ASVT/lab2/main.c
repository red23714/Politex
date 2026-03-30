#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIGNALS 10
#define MAX_POINTS 1000
#define TIME_QUANTUM_MS 100 // 10 мс на точку

typedef struct
{
	char name[32];
	int* data; // массив значений 0/1
	int length;
	Color color;
	bool editing; // режим редактирования имени
} DigitalSignal;

typedef struct
{
	DigitalSignal signals[MAX_SIGNALS];
	int signalCount;
	int timeWindow; // сколько точек показывать на экране
	float scrollOffset;
	bool showUI; // показывать ли UI панель управления
} AppState;

// Инициализация приложения
void InitApp(AppState* state)
{
	state->signalCount = 0;
	state->timeWindow = 100; // показываем 100 точек (1 секунда при 10 мс)
	state->scrollOffset = 0;
	state->showUI = true;
}

// Добавление нового сигнала
int AddSignal(AppState* state, const char* name, Color color)
{
	if (state->signalCount >= MAX_SIGNALS)
		return -1;

	DigitalSignal* sig = &state->signals[state->signalCount];
	strncpy(sig->name, name, 31);
	sig->color = color;
	sig->length = MAX_POINTS;
	sig->data = (int*)malloc(MAX_POINTS * sizeof(int));
	sig->editing = false;

	// Инициализируем нулями
	for (int i = 0; i < MAX_POINTS; i++)
	{
		sig->data[i] = 0;
	}

	state->signalCount++;
	return state->signalCount - 1;
}

// Установка значений сигнала из массива
void SetSignalPattern(DigitalSignal* sig, int* pattern, int patternLen,
					  int startPos)
{
	for (int i = 0; i < patternLen && (startPos + i) < sig->length; i++)
	{
		sig->data[startPos + i] = pattern[i];
	}
}

// Рисование цифрового сигнала (с вертикальными переходами)
void DrawDigitalSignal(DigitalSignal* sig, int startX, int startY, int width,
					   int height, int scrollOffset, int timeWindow)
{
	int prevValue = sig->data[scrollOffset];
	int prevX = startX;
	int prevY = startY + (1 - prevValue) * height;

	int maxPoints = (scrollOffset + timeWindow < sig->length)
						? scrollOffset + timeWindow
						: sig->length;

	for (int i = scrollOffset + 1; i < maxPoints; i++)
	{
		int currentValue = sig->data[i];
		int x = startX + ((i - scrollOffset) * width) / timeWindow;

		if (currentValue != prevValue)
		{
			// Рисуем вертикальную линию перехода
			int transitionY = startY + (1 - currentValue) * height;
			DrawLine(prevX, prevY, prevX, transitionY, sig->color);
			DrawLine(prevX, transitionY, x, transitionY, sig->color);
			prevY = transitionY;
		}
		else
		{
			// Рисуем горизонтальную линию
			int y = startY + (1 - currentValue) * height;
			DrawLine(prevX, prevY, x, y, sig->color);
			prevY = y;
		}

		prevX = x;
		prevValue = currentValue;

		// Рисуем точки на переходах
		DrawCircle(x, prevY, 3, sig->color);
	}
}

// Рисование графика
void DrawSignals(AppState* state, int screenWidth, int screenHeight)
{
	int leftPanelWidth = 150; // Ширина для названий
	int graphWidth = screenWidth - leftPanelWidth - 20;
	int graphHeight = screenHeight - 100; // Место для временной оси
	int graphX = leftPanelWidth + 10;
	int graphY = 50;

	// Рисуем фон
	DrawRectangle(graphX, graphY, graphWidth, graphHeight, RAYWHITE);
	DrawRectangleLines(graphX, graphY, graphWidth, graphHeight, DARKGRAY);

	// Рисуем горизонтальные линии сетки
	for (int i = 0; i <= state->signalCount; i++)
	{
		int y = graphY + (i * graphHeight) / state->signalCount;
		DrawLine(graphX, y, graphX + graphWidth, y, LIGHTGRAY);
	}

	// Рисуем вертикальные линии сетки и метки времени (всегда показываем)
	for (int t = 0; t <= state->timeWindow; t += 20)
	{
		int x = graphX + (t * graphWidth) / state->timeWindow;
		DrawLine(x, graphY, x, graphY + graphHeight, LIGHTGRAY);

		char timeText[32];
		sprintf(timeText, "%dms",
				(int)((t + state->scrollOffset) * TIME_QUANTUM_MS));
		DrawText(timeText, x - 20, graphY + graphHeight + 5, 12, DARKGRAY);
	}

	// Подпись временной оси
	DrawText("Time (ms)", graphX + graphWidth / 2 - 40,
			 graphY + graphHeight + 25, 15, DARKGRAY);

	// Рисуем каждый сигнал
	int signalHeight = graphHeight / state->signalCount;

	for (int s = 0; s < state->signalCount; s++)
	{
		DigitalSignal* sig = &state->signals[s];
		int sigY = graphY + s * signalHeight;
		int sigHeight = signalHeight - 8; // Небольшой отступ между сигналами

		// Цветная полоска и название сигнала слева (всегда показываем)
		DrawRectangle(graphX - 5, sigY, 5, sigHeight, sig->color);

		// Редактирование имени или отображение
		if (sig->editing)
		{
			// Поле для ввода имени
			Rectangle textBox =
				(Rectangle){graphX - 145, sigY + sigHeight / 2 - 12, 135, 24};
			if (GuiTextBox(textBox, sig->name, 31, true))
			{
				sig->editing = false;
			}
		}
		else
		{
			// Обычное отображение имени
			DrawText(sig->name, graphX - 140, sigY + sigHeight / 2 - 10, 18,
					 sig->color);

			// Кнопка редактирования имени (только если UI показан)
			if (state->showUI)
			{
				Rectangle editBtn =
					(Rectangle){graphX - 25, sigY + sigHeight / 2 - 8, 18, 16};
				if (GuiButton(editBtn, "E"))
				{
					sig->editing = true;
				}
			}
		}

		// Рисуем цифровой сигнал
		DrawDigitalSignal(sig, graphX, sigY, graphWidth, sigHeight,
						  (int)state->scrollOffset, state->timeWindow);
	}

	// Рисуем подписи уровней (HIGH/LOW) справа
	if (state->showUI)
	{
		for (int s = 0; s < state->signalCount; s++)
		{
			DigitalSignal* sig = &state->signals[s];
			int sigY = graphY + s * signalHeight;
			int sigHeight = signalHeight - 8;

			int currentIndex = (int)state->scrollOffset;
			int currentValue =
				(currentIndex < sig->length) ? sig->data[currentIndex] : 0;
			DrawText(currentValue ? "HIGH" : "LOW", graphX + graphWidth + 5,
					 sigY + sigHeight / 2 - 8, 12, sig->color);
		}
	}
}

// UI для управления сигналами
void DrawUI(AppState* state, int screenWidth, int screenHeight)
{
	if (!state->showUI)
		return;

	// Верхняя панель
	DrawRectangle(0, 0, screenWidth, 40, Fade(LIGHTGRAY, 0.9f));
	DrawRectangleLines(0, 0, screenWidth, 40, DARKGRAY);

	DrawText("DIGITAL SIGNAL VISUALIZER", 10, 10, 20, DARKBLUE);

	// Кнопки управления
	if (GuiButton((Rectangle){250, 8, 120, 24}, "Add New Signal"))
	{
		char newName[32];
		sprintf(newName, "Signal %d", state->signalCount + 1);
		Color colors[] = {RED,	 GREEN, BLUE, ORANGE,  PURPLE,
						  BROWN, PINK,	LIME, SKYBLUE, VIOLET};
		AddSignal(state, newName, colors[state->signalCount % 10]);
	}

	if (GuiButton((Rectangle){380, 8, 100, 24}, "Hide UI (H)"))
	{
		state->showUI = false;
	}

	// Настройка времени окна
	DrawText("Window:", 500, 13, 15, DARKGRAY);
	if (GuiButton((Rectangle){560, 8, 35, 24}, "-10"))
	{
		state->timeWindow =
			(state->timeWindow > 10) ? state->timeWindow - 10 : 10;
	}
	char windowText[32];
	sprintf(windowText, "%d", state->timeWindow);
	DrawText(windowText, 605, 13, 15, DARKBLUE);
	if (GuiButton((Rectangle){630, 8, 35, 24}, "+10"))
	{
		state->timeWindow = (state->timeWindow + 10 <= MAX_POINTS)
								? state->timeWindow + 10
								: MAX_POINTS;
	}

	// Прокрутка
	DrawText("Scroll:", 690, 13, 15, DARKGRAY);
	if (GuiButton((Rectangle){740, 8, 35, 24}, "<--"))
	{
		state->scrollOffset =
			(state->scrollOffset > 0) ? state->scrollOffset - 10 : 0;
	}
	if (GuiButton((Rectangle){780, 8, 35, 24}, "-->"))
	{
		if (state->scrollOffset + state->timeWindow < MAX_POINTS)
		{
			state->scrollOffset += 10;
		}
	}

	// Информация
	DrawText("Time Quantum: 10ms", screenWidth - 150, 13, 15, DARKGRAY);

	// Инструкции внизу
	DrawRectangle(0, screenHeight - 30, screenWidth, 30, Fade(LIGHTGRAY, 0.9f));
	DrawRectangleLines(0, screenHeight - 30, screenWidth, 30, DARKGRAY);
	DrawText("Click on signal to draw | H - Hide/Show UI | +/- - Zoom | Arrows "
			 "- Scroll | E - Edit name",
			 10, screenHeight - 22, 14, DARKGRAY);
}

int main(void)
{
	const int screenWidth = 1200;
	const int screenHeight = 700;

	InitWindow(screenWidth, screenHeight, "Digital Signal Visualizer");
	SetTargetFPS(60);

	// Инициализация Gui
	GuiLoadStyleDefault();

	AppState state;
	InitApp(&state);

	// Создаем пример сигнала для демонстрации
	AddSignal(&state, "CLOCK", RED);
	AddSignal(&state, "DATA", GREEN);
	AddSignal(&state, "ENABLE", BLUE);

	// Пример паттернов для сигналов
	int clockPattern[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
						  1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
	int dataPattern[] = {1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
						 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
	int enablePattern[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1,
						   1, 1, 0, 0, 0, 1, 1, 1, 0, 0};

	SetSignalPattern(&state.signals[0], clockPattern, 20, 0);
	SetSignalPattern(&state.signals[1], dataPattern, 20, 0);
	SetSignalPattern(&state.signals[2], enablePattern, 20, 0);

	// Интерактивное редактирование сигналов
	int selectedSignal = -1;
	bool isDrawing = false;

	while (!WindowShouldClose())
	{
		// Обработка клавиатуры
		if (IsKeyPressed(KEY_H))
		{
			state.showUI = !state.showUI;
		}
		if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL))
		{
			state.timeWindow = (state.timeWindow + 10 <= MAX_POINTS)
								   ? state.timeWindow + 10
								   : MAX_POINTS;
		}
		if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS))
		{
			state.timeWindow =
				(state.timeWindow > 10) ? state.timeWindow - 10 : 10;
		}
		if (IsKeyPressed(KEY_LEFT))
		{
			state.scrollOffset =
				(state.scrollOffset > 0) ? state.scrollOffset - 10 : 0;
		}
		if (IsKeyPressed(KEY_RIGHT))
		{
			if (state.scrollOffset + state.timeWindow < MAX_POINTS)
			{
				state.scrollOffset += 10;
			}
		}

		// Обработка мыши для рисования сигналов
		Vector2 mousePos = GetMousePosition();
		int leftPanelWidth = 150;
		int graphWidth = screenWidth - leftPanelWidth - 20;
		int graphHeight = screenHeight - 100;
		int graphX = leftPanelWidth + 10;
		int graphY = 50;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			// Проверяем, не кликнули ли по области графика
			if (mousePos.x >= graphX && mousePos.x <= graphX + graphWidth &&
				mousePos.y >= graphY && mousePos.y <= graphY + graphHeight)
			{

				int signalHeight = graphHeight / state.signalCount;
				int signalIndex = (mousePos.y - graphY) / signalHeight;

				if (signalIndex >= 0 && signalIndex < state.signalCount)
				{
					selectedSignal = signalIndex;
					isDrawing = true;
				}
			}
		}

		// Рисование сигнала
		if (isDrawing && IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
			selectedSignal >= 0)
		{
			int signalHeight = graphHeight / state.signalCount;
			int sigY = graphY + selectedSignal * signalHeight;
			int sigHeight = signalHeight - 8;

			if (mousePos.x >= graphX && mousePos.x <= graphX + graphWidth &&
				mousePos.y >= sigY && mousePos.y <= sigY + sigHeight)
			{

				// Определяем индекс точки с учетом прокрутки
				int pointIndex =
					((mousePos.x - graphX) * state.timeWindow) / graphWidth;
				pointIndex += (int)state.scrollOffset;

				if (pointIndex >= 0 && pointIndex < MAX_POINTS)
				{
					// Верхняя половина - HIGH (1), нижняя половина - LOW (0)
					int value = (mousePos.y - sigY) < (sigHeight / 2) ? 1 : 0;
					state.signals[selectedSignal].data[pointIndex] = value;
				}
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			isDrawing = false;
			selectedSignal = -1;
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawSignals(&state, screenWidth, screenHeight);
		DrawUI(&state, screenWidth, screenHeight);

		EndDrawing();
	}

	// Очистка памяти
	for (int i = 0; i < state.signalCount; i++)
	{
		free(state.signals[i].data);
	}

	CloseWindow();
	return 0;
}
