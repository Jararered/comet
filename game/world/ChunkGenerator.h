#pragma once

#include <comet.pch>

class ChunkGenerator
{
  public:
    static ChunkGenerator &Instance()
    {
        static ChunkGenerator s_Instance;
        return s_Instance;
    }

    static void Initialize()
    {
        Instance().m_MediumNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
        Instance().m_MediumNoise.SetFrequency(0.01f);

        Instance().m_SlowNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
        Instance().m_SlowNoise.SetFrequency(0.001f);

        Instance().m_CellularNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Cellular);
        Instance().m_CellularNoise.SetFrequency(0.01f);

        Instance().m_FastNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
        Instance().m_FastNoise.SetFrequency(1.0f);

        Instance().m_MediumChaotic.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
        Instance().m_MediumChaotic.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
        Instance().m_MediumChaotic.SetFractalOctaves(4);
        Instance().m_MediumChaotic.SetFractalLacunarity(2.00f);
        Instance().m_MediumChaotic.SetFrequency(0.01f);

        Instance().m_BiomeNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Value);
        Instance().m_BiomeNoise.SetFrequency(0.015f);
        Instance().m_BiomeNoise.SetCellularDistanceFunction(
            FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Hybrid);
        Instance().m_BiomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType::CellularReturnType_CellValue);
        Instance().m_BiomeNoise.SetDomainWarpType(FastNoiseLite::DomainWarpType::DomainWarpType_BasicGrid);
        Instance().m_BiomeNoise.SetDomainWarpAmp(50.0f);

        Instance().m_CaveNoise.SetFrequency(0.01f);
        Instance().m_CaveNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
        Instance().m_CaveNoise.SetFractalType(FastNoiseLite::FractalType::FractalType_PingPong);
        Instance().m_CaveNoise.SetFractalOctaves(5);
        Instance().m_CaveNoise.SetFractalLacunarity(2.0f);
        Instance().m_CaveNoise.SetFractalGain(0.05f);
        Instance().m_CaveNoise.SetFractalWeightedStrength(0.0f);
        Instance().m_CaveNoise.SetFractalPingPongStrength(2.0f);

        Instance().m_OpenSimplex2_1f.SetFrequency(0.004f);
        Instance().m_OpenSimplex2_2f.SetFrequency(0.008f);
        Instance().m_OpenSimplex2_4f.SetFrequency(0.016f);
        Instance().m_OpenSimplex2_8f.SetFrequency(0.032f);
        Instance().m_OpenSimplex2_16f.SetFrequency(0.064f);
    }

    static float GetPerlin1(int x, int z)
    {
        return Instance().m_OpenSimplex2_1f.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetPerlin2(int x, int z)
    {
        return Instance().m_OpenSimplex2_2f.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetPerlin4(int x, int z)
    {
        return Instance().m_OpenSimplex2_4f.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetPerlin8(int x, int z)
    {
        return Instance().m_OpenSimplex2_8f.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetPerlin16(int x, int z)
    {
        return Instance().m_OpenSimplex2_16f.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }

    static float GetCaveNoise(int x, int y, int z)
    {
        return Instance().m_CaveNoise.GetNoise(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
    }
    static float GetBiomeNoise(int x, int z)
    {
        return Instance().m_BiomeNoise.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetMediumChaotic(int x, int z)
    {
        return Instance().m_MediumChaotic.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetMediumNoise(int x, int z)
    {
        return Instance().m_MediumNoise.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetSlowNoise(int x, int z)
    {
        return Instance().m_SlowNoise.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }
    static float GetCellularNoise(int x, int y)
    {
        return Instance().m_CellularNoise.GetNoise(static_cast<float>(x), static_cast<float>(y));
    }
    static float GetFastNoise(int x, int z)
    {
        return Instance().m_FastNoise.GetNoise(static_cast<float>(x), static_cast<float>(z));
    }

    static void GenerateChunk(const glm::ivec3 &chunkIndex, const std::vector<unsigned char> *blockData)
    {
        // move all chunk generation into here eventually
    }

    static int Seed() { return Instance().m_Seed; }
    static void SetSeed(int seed)
    {
        Instance().m_Seed = seed;
        Instance().m_MediumNoise.SetSeed(seed);
        Instance().m_SlowNoise.SetSeed(seed);
        Instance().m_CellularNoise.SetSeed(seed);
        Instance().m_FastNoise.SetSeed(seed);
        Instance().m_MediumChaotic.SetSeed(seed);
        Instance().m_BiomeNoise.SetSeed(seed);
        Instance().m_CaveNoise.SetSeed(seed);
        Instance().m_OpenSimplex2_1f.SetSeed(seed);
        Instance().m_OpenSimplex2_2f.SetSeed(seed);
        Instance().m_OpenSimplex2_4f.SetSeed(seed);
        Instance().m_OpenSimplex2_8f.SetSeed(seed);
        Instance().m_OpenSimplex2_16f.SetSeed(seed);
    }

  private:
    ChunkGenerator() {}
    ChunkGenerator(ChunkGenerator const &);
    void operator=(ChunkGenerator const &);

    int m_Seed = 0;

    FastNoiseLite m_MediumNoise;
    FastNoiseLite m_SlowNoise;
    FastNoiseLite m_CellularNoise;
    FastNoiseLite m_FastNoise;
    FastNoiseLite m_MediumChaotic;
    FastNoiseLite m_BiomeNoise;
    FastNoiseLite m_CaveNoise;

    FastNoiseLite m_OpenSimplex2_1f;
    FastNoiseLite m_OpenSimplex2_2f;
    FastNoiseLite m_OpenSimplex2_4f;
    FastNoiseLite m_OpenSimplex2_8f;
    FastNoiseLite m_OpenSimplex2_16f;

  public:
};
