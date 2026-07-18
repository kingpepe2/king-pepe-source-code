// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <crypto/hex_base.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/log.h>
#include <util/strencodings.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <optional>
#include <span>
#include <utility>

using namespace util::hex_literals;

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Pow are stronger than fiat";
    const CScript genesisOutputScript = CScript() << "0497da2bffaf2c7fee8c80b91fcfa6562eb6d8c757d2d04fdeba7fe97cb3d7e7d6b9ccd3d2a4f938fe7cd05bf5c3eaa30ccc8ade3fec0b51bf6f110f16fab0bba0"_hex << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static void SetOneMinutePow(Consensus::Params& consensus)
{
    consensus.nPowTargetTimespan = 120 * 60;
    consensus.nPowTargetSpacing = 1 * 60;
}

static void SetDeployment(Consensus::BIP9Deployment& deployment, int bit, int64_t start_time, int64_t timeout, int min_activation_height, uint32_t threshold, uint32_t period)
{
    deployment.bit = bit;
    deployment.nStartTime = start_time;
    deployment.nTimeout = timeout;
    deployment.min_activation_height = min_activation_height;
    deployment.threshold = threshold;
    deployment.period = period;
}

class CMainParams : public CChainParams {
public:
    CMainParams()
    {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 0;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        SetOneMinutePow(consensus);
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY], 28, Consensus::BIP9Deployment::NEVER_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 108, 120);
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT], 2, Consensus::BIP9Deployment::ALWAYS_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 108, 120);
        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000000000000000100010"};
        consensus.defaultAssumeValid = uint256{"00000a00a75c7ed12c71b9a8b73c01576009d62a0a606c0a1ef37b043c520fb2"};

        pchMessageStart = {0xf3, 0xea, 0xce, 0x21};
        nDefaultPort = 24028;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 5;
        m_assumed_chain_state_size = 5;

        genesis = CreateGenesisBlock(1749678364, 835537, 0x1e0ffff0, 1, 3 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000a00a75c7ed12c71b9a8b73c01576009d62a0a606c0a1ef37b043c520fb2"});
        assert(genesis.hashMerkleRoot == uint256{"45319425eda8b91d62440dd4372699f35509076c592e1f8ea707780335e60e89"});

        vSeeds.emplace_back("node3.walletbuilders.com");
        vFixedSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 51);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 50);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 178);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};
        bech32_hrp = "kpepe";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
        m_assumeutxo_data = {};
        chainTxData = ChainTxData{.nTime = 1749678364, .tx_count = 0, .dTxRate = 0};
        m_headers_sync_params = HeadersSyncParams{.commitment_period = 641, .redownload_buffer_size = 15218};
    }
};

class CTestNetParams : public CChainParams {
public:
    CTestNetParams()
    {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 581885;
        consensus.BIP66Height = 330776;
        consensus.CSVHeight = 770112;
        consensus.SegwitHeight = 834624;
        consensus.MinBIP9WarningHeight = 836640;
        consensus.powLimit = uint256{"00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        SetOneMinutePow(consensus);
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY], 28, Consensus::BIP9Deployment::NEVER_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 90, 120);
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT], 2, Consensus::BIP9Deployment::ALWAYS_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 90, 120);
        consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000000000000000100010"};
        consensus.defaultAssumeValid = uint256{"00000ed3b05b40fe3404c726bc7050f1aee1a6e358bf996acc37687e5fa1d680"};

        pchMessageStart = {0x40, 0x7a, 0x52, 0xef};
        nDefaultPort = 34028;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 42;
        m_assumed_chain_state_size = 3;

        genesis = CreateGenesisBlock(1749678369, 33050, 0x1e0ffff0, 1, 3 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"00000ed3b05b40fe3404c726bc7050f1aee1a6e358bf996acc37687e5fa1d680"});
        assert(genesis.hashMerkleRoot == uint256{"45319425eda8b91d62440dd4372699f35509076c592e1f8ea707780335e60e89"});

        vSeeds.clear();
        vFixedSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 64);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 63);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 191);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        bech32_hrp = "tkpepe";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
        m_assumeutxo_data = {};
        chainTxData = ChainTxData{.nTime = 1749678369, .tx_count = 0, .dTxRate = 0};
        m_headers_sync_params = HeadersSyncParams{.commitment_period = 673, .redownload_buffer_size = 14460};
    }
};

class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vFixedSeeds.clear();
        vSeeds.clear();

        if (!options.challenge) {
            bin = "512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae"_hex_v_u8;
            consensus.nMinimumChainWork = uint256{"0000000000000000000000000000000000000000000000000000000000100010"};
            consensus.defaultAssumeValid = uint256{"0000023cdaf6350f96cd4bbacfa18f1509502d5908958ac21fef4017631ea137"};
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{.nTime = 1749678374, .tx_count = 0, .dTxRate = 0};
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{.nTime = 0, .tx_count = 0, .dTxRate = 0};
            LogInfo("Signet with challenge %s", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000377ae000000000000000000000000000000000000000000000000000000"};
        SetOneMinutePow(consensus);
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY], 28, Consensus::BIP9Deployment::NEVER_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 108, 120);
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT], 2, Consensus::BIP9Deployment::ALWAYS_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 108, 120);

        HashWriter h{};
        h << consensus.signet_challenge;
        const uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 61884;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1749678374, 1317451, 0x1e0377ae, 1, 3 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"0000023cdaf6350f96cd4bbacfa18f1509502d5908958ac21fef4017631ea137"});
        assert(genesis.hashMerkleRoot == uint256{"45319425eda8b91d62440dd4372699f35509076c592e1f8ea707780335e60e89"});

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 64);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 63);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 191);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        bech32_hrp = "tkpepe";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
        m_assumeutxo_data = {};
        m_headers_sync_params = HeadersSyncParams{.commitment_period = 620, .redownload_buffer_size = 15724};
    }
};

class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        SetOneMinutePow(consensus);
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = opts.enforce_bip94;
        consensus.fPowNoRetargeting = true;
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY], 28, 0, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 108, 144);
        SetDeployment(consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT], 2, Consensus::BIP9Deployment::ALWAYS_ACTIVE, Consensus::BIP9Deployment::NO_TIMEOUT, 0, 108, 144);
        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart = {0xfe, 0x30, 0x25, 0x64};
        nDefaultPort = 18444;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1749678369, 5, 0x207fffff, 1, 3 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"352a1a62f7880d325da6d3fe2e62272cd0ce735a7ba003eae4fb59d2a175a8b9"});
        assert(genesis.hashMerkleRoot == uint256{"45319425eda8b91d62440dd4372699f35509076c592e1f8ea707780335e60e89"});

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 64);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 63);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 191);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        bech32_hrp = "rkpepe";

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;
        m_assumeutxo_data = {};
        chainTxData = ChainTxData{.nTime = 0, .tx_count = 0, .dTxRate = 0};
        m_headers_sync_params = HeadersSyncParams{.commitment_period = 275, .redownload_buffer_size = 7017};
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::ranges::equal(message, mainnet_msg)) {
        return ChainType::MAIN;
    } else if (std::ranges::equal(message, testnet_msg)) {
        return ChainType::TESTNET;
    } else if (std::ranges::equal(message, regtest_msg)) {
        return ChainType::REGTEST;
    } else if (std::ranges::equal(message, signet_msg)) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
