#include <iostream>
#include <chrono>
#include <vector>
#include <bits/stdc++.h>
#include <Windows.h>
#include <math.h>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerSway = 0.0;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.0;
float fDepth = 16.0f;

float fPlayerDist = 0.0;

POINT cursorPos;
POINT lastCursorPos;

int main()
{
    // Create Screen Buffer
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..####........#";
    map += L"#..####........#";
    map += L"#..####........#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.........######";
    map += L"#..............#";
    map += L"######.........#";
    map += L"######.........#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    while (1)
    {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        GetCursorPos(&cursorPos);
        int relativeX = cursorPos.x - 500;
        int relativeY = cursorPos.y - 500;
        SetCursorPos(500, 500);

        fPlayerA += relativeX * fElapsedTime;

        // if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        //     fPlayerA -= 2.0f * fElapsedTime;

        // if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        //     fPlayerA += 2.0f * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            // forward movement
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;

            fPlayerDist += 5.0f;

            // sway movement
            fPlayerX += sinf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;
            fPlayerY += cosf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;

            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;

                fPlayerX -= sinf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;
                fPlayerY -= cosf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            // backwards movement
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;

            // sway movement
            fPlayerX += sinf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;
            fPlayerY += cosf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;

            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
            {
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;

                fPlayerX -= sinf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;
                fPlayerY -= cosf(fPlayerA + 3.14159f / 2.0f) * 1.0f * sinf(fPlayerDist / 1000.0f) * fElapsedTime;
            }
        }

        for (int x = 0; x < nScreenWidth; x++)
        {
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistToWall < fDepth)
            {
                fDistToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistToWall);

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistToWall = fDepth;
                }
                else
                {
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;

                        vector<pair<float, float>> p; // distance, dot

                        for (int tx = 0; tx < 2; tx++)
                        {
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }
                        }

                        sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right)
                             { return left.first < right.first; });

                        float fBound = 0.01f;
                        if (acos(p.at(0).second) < fBound)
                            bBoundary = true;
                        if (acos(p.at(1).second) < fBound)
                            bBoundary = true;
                        // if(acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistToWall);
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';
            if (fDistToWall <= fDepth / 4.0f)
                nShade = 0x2588; // Very close
            else if (fDistToWall < fDepth / 3.0f)
                nShade = 0x2593;
            else if (fDistToWall < fDepth / 2.0f)
                nShade = 0x2592;
            else if (fDistToWall < fDepth)
                nShade = 0x2591;
            else
                nShade = ' '; // Too far away

            if (bBoundary)
                nShade = ' ';

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y < nCeiling)
                {
                    screen[y * nScreenWidth + x] = ' ';
                }
                else if (y > nCeiling && y < nFloor)
                {
                    screen[y * nScreenWidth + x] = nShade;
                }
                else
                {
                    // Shade floor based on distance
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)
                        nShade = '#';
                    else if (b < 0.5)
                        nShade = 'x';
                    else if (b < 0.75)
                        nShade = '.';
                    else if (b < 0.9)
                        nShade = '-';
                    // else				nShade = ' ';
                    screen[y * nScreenWidth + x] = nShade;
                }
            }
        }

        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);
        swprintf_s(screen + nScreenWidth + 1, 120, L"relativeX=%3.2f, relativeY=%3.2f, absX=%3.2f, absY=%3.2f", (float)relativeX, (float)relativeY, (float)cursorPos.x, (float)cursorPos.y);

        for (int nx = 0; nx < nMapWidth; nx++)
        {
            for (int ny = 0; ny < nMapHeight; ny++)
            {
                screen[(ny + 2) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
            }
        }

        screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';

        screen[nScreenHeight * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
    }

    return 0;
}