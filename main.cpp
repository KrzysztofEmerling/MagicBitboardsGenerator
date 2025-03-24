#include <iostream>
#include <fstream>
#include <filesystem>
#include "../BitboardsUtils.h"

u64 bitscanForward(u64 mask) {
    return __builtin_ctzll(mask);
}

u64 rayAttack(int x, int y, int dx, int dy, u64 blockers) {
    u64 moves = 0;
    int nx = x + dx, ny = y + dy;
    
    while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
        u64 pos = 1ULL << (nx + ny * 8);
        if (pos & blockers) break;
        moves |= pos;
        nx += dx; ny += dy;
    }
    
    return moves;
}

u64 generateRookBlockersMask(int square) {
    u64 mask = 0;
    int x = square % 8;
    int y = square / 8;
    
    for(int i = y+1; i < 7; i++) mask |= 1ULL << (x + i*8);
    for(int i = y-1; i > 0; i--) mask |= 1ULL << (x + i*8);
    for(int i = x+1; i < 7; i++) mask |= 1ULL << (i + y*8);
    for(int i = x-1; i > 0; i--) mask |= 1ULL << (i + y*8);
    
    return mask;
}

u64 generateBishopBlockersMask(int square) {
    u64 mask = 0;
    int x = square % 8;
    int y = square / 8;
    
    for(int i = 1; i < 7; i++) {
        if (x + i < 7 && y + i < 7) mask |= 1ULL << ((x + i) + (y + i) * 8);
        if (x - i > 0 && y + i < 7) mask |= 1ULL << ((x - i) + (y + i) * 8);
        if (x + i < 7 && y - i > 0) mask |= 1ULL << ((x + i) + (y - i) * 8);
        if (x - i > 0 && y - i > 0) mask |= 1ULL << ((x - i) + (y - i) * 8);
    }
    
    return mask;
}

u64 createBlockers(int subset, u64 mask) {
    u64 blockers = 0;
    int n = std::popcount(mask);
    
    for(int i = 0; i < n; i++) {
        int bitPos = bitscanForward(mask);
        mask &= mask - 1;
        
        if(subset & (1 << i))
            blockers |= 1ULL << bitPos;
    }
    return blockers;
}

u64 calculateRookMoves(int square, u64 blockers) {
    u64 moves = 0;
    int x = square % 8;
    int y = square / 8;
    
    moves |= rayAttack(x, y, 0, 1, blockers);
    moves |= rayAttack(x, y, 0, -1, blockers);
    moves |= rayAttack(x, y, 1, 0, blockers);
    moves |= rayAttack(x, y, -1, 0, blockers);
    
    return moves;
}

u64 calculateBishopMoves(int square, u64 blockers) {
    u64 moves = 0;
    int x = square % 8;
    int y = square / 8;
    
    moves |= rayAttack(x, y, 1, 1, blockers);
    moves |= rayAttack(x, y, -1, 1, blockers);
    moves |= rayAttack(x, y, 1, -1, blockers);
    moves |= rayAttack(x, y, -1, -1, blockers);
    
    return moves;
}

void GenerateRookTable() {
    std::ofstream plik;
    for(int square = 0; square < 64; square++) {
        std::string filename = "./intermidiets/RawRook" + std::to_string(square) + ".bin";
        if (std::filesystem::exists(filename)) continue;
        
        plik.open(filename, std::ios::binary);
        
        u64 blockersMask = generateRookBlockersMask(square);
        int bitCount = std::popcount(blockersMask);
        int combinations = 1 << bitCount;
        
        for(int subset = 0; subset < combinations; subset++) {
            u64 blockers = createBlockers(subset, blockersMask);
            u64 moves = calculateRookMoves(square, blockers);
            plik.write(reinterpret_cast<char*>(&blockers), sizeof(u64));
            plik.write(reinterpret_cast<char*>(&moves), sizeof(u64));
        }
        plik.close();
    }
}

void GenerateBishopTable() {
    std::ofstream plik;
    for(int square = 0; square < 64; square++) {
        std::string filename = "./intermidiets/RawBishop" + std::to_string(square) + ".bin";
        if (std::filesystem::exists(filename)) continue;
        
        plik.open(filename, std::ios::binary);
        
        u64 blockersMask = generateBishopBlockersMask(square);
        int bitCount = std::popcount(blockersMask);
        int combinations = 1 << bitCount;
        
        for(int subset = 0; subset < combinations; subset++) {
            u64 blockers = createBlockers(subset, blockersMask);
            u64 moves = calculateBishopMoves(square, blockers);
            plik.write(reinterpret_cast<char*>(&blockers), sizeof(u64));
            plik.write(reinterpret_cast<char*>(&moves), sizeof(u64));
        }
        plik.close();
    }
}

int main() {
    GenerateRookTable();
    GenerateBishopTable();
}
