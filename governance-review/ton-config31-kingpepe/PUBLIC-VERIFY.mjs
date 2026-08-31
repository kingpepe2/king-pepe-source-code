// SPDX-License-Identifier: MIT

import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

import { beginCell, Cell, Dictionary } from "@ton/core";

const directory = dirname(fileURLToPath(import.meta.url));
const candidate = "-1:a6e0023cfde6efd21c8de273b9a5fd67b07fd2c513f312504b44d6ff7c906108";
const proposalId = "E8131C85497C998C8FCFDEE823637C32EBB606DF73BE7CC9653F1BC11B7229E9";
const configAddress = `-1:${"55".repeat(32)}`;
const beforeHash = "4E65911BAF281EC692281255642BE840A938D6DB2162DF266ADF7806337D3FBE";
const afterHash = "F9B2410C47BDB8C2167F005C905F814F2B4593E143B533978ACD48839D4677FD";

const trueValue = {
  serialize(_source, _builder) {},
  parse(_source) { return true; },
};

const inlineCellValue = {
  serialize(source, builder) { builder.storeSlice(source.beginParse()); },
  parse(source) { return source.asCell(); },
};

function json(name) {
  return JSON.parse(readFileSync(resolve(directory, name), "utf8").replace(/^\uFEFF/u, ""));
}

function oneRoot(name) {
  const roots = Cell.fromBoc(readFileSync(resolve(directory, name)));
  assert.equal(roots.length, 1, `${name} must contain exactly one BOC root`);
  return roots[0];
}

function hash(cell) {
  return cell.hash().toString("hex").toUpperCase();
}

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex").toUpperCase();
}

function hex256(value) {
  return value.toString(16).padStart(64, "0").toUpperCase();
}

function config31Entries(cell) {
  const slice = cell.beginParse();
  const dictionary = slice.loadDict(Dictionary.Keys.BigUint(256), trueValue);
  slice.endParse();
  return [...dictionary.keys()]
    .map((key) => `-1:${key.toString(16).padStart(64, "0")}`)
    .sort();
}

const live = json("OUTREACH-LIVE-STATE.json");
const diff = json("CONFIG31-DIFF.json");
const ledger = json("VALIDATOR-SUPPORT-LEDGER.json");

const before = oneRoot("config31-before.boc");
const after = oneRoot("config31-after.boc");
const proposalCell = oneRoot("proposal-cell.boc");
const config11File = oneRoot("config11.boc");
const config34File = oneRoot("current-validator-set-config34.boc");
const accountDataBytes = readFileSync(resolve(directory, "config-account-data.boc"));
const accountData = Cell.fromBoc(accountDataBytes);
assert.equal(accountData.length, 1, "config-account-data.boc must contain exactly one BOC root");

assert.equal(hash(before), beforeHash);
assert.equal(hash(after), afterHash);
assert.equal(hash(proposalCell), proposalId);
assert.equal(sha256(accountDataBytes), live.rawAccountData.sha256);
assert.equal(hash(accountData[0]), live.rawAccountData.cellHash);

const beforeEntries = config31Entries(before);
const afterEntries = config31Entries(after);
assert.equal(beforeEntries.includes(candidate), false);
assert.deepEqual(afterEntries.filter((entry) => !beforeEntries.includes(entry)), [candidate]);
assert.deepEqual(beforeEntries.filter((entry) => !afterEntries.includes(entry)), []);
assert.deepEqual(diff.before.entries, beforeEntries);
assert.deepEqual(diff.after.entries, afterEntries);
assert.deepEqual(diff.added, [candidate]);
assert.deepEqual(diff.removed, []);
assert.deepEqual(diff.changedConfigParameters, [31]);
assert.equal(diff.exactSingleChangeVerified, true);

const proposal = proposalCell.beginParse();
assert.equal(proposal.loadUint(8), 0xf3, "proposal must use cfg_proposal#f3");
assert.equal(proposal.loadInt(32), 31, "proposal must change only Config 31");
assert.equal(proposal.loadBit(), true, "proposal must add/replace a value, not delete Config 31");
const proposedValue = proposal.loadRef();
assert.equal(proposal.loadBit(), true, "proposal must include a current-hash guard");
const oldHashGuard = proposal.loadBuffer(32).toString("hex").toUpperCase();
proposal.endParse();
assert.equal(oldHashGuard, beforeHash);
assert.equal(hash(proposedValue), afterHash);
assert.deepEqual(config31Entries(proposedValue), afterEntries);

const data = accountData[0].beginParse();
const configRoot = data.loadRef();
const configContractSeqno = data.loadUint(32);
data.loadUintBig(256);
const proposals = data.loadDict(Dictionary.Keys.BigUint(256), inlineCellValue);
data.endParse();
assert.equal(configContractSeqno, live.configContractSeqno);
assert.equal(proposals.size, 0, "no new proposal occurrence may be active during pre-submission review");
assert.equal(proposals.has(BigInt(`0x${proposalId}`)), false);

const config = configRoot.beginParse().loadDictDirect(Dictionary.Keys.Int(32), Dictionary.Values.Cell());
const config0 = config.get(0);
const config11 = config.get(11);
const config31 = config.get(31);
const config34 = config.get(34);
assert(config0 !== undefined && config11 !== undefined && config31 !== undefined && config34 !== undefined);
assert.equal(`-1:${config0.beginParse().loadBuffer(32).toString("hex")}`, configAddress);
assert.equal(hash(config11), hash(config11File));
assert.equal(hash(config31), beforeHash);
assert.equal(hash(config34), hash(config34File));
assert.equal(config.get(36), undefined, "Config 36 must remain unavailable/not counted");
assert.deepEqual(config31Entries(config31), beforeEntries);

const voteSetup = config11.beginParse();
assert.equal(voteSetup.loadUint(8), 0x91);
const normal = voteSetup.loadRef().beginParse();
voteSetup.loadRef();
voteSetup.endParse();
assert.equal(normal.loadUint(8), 0x36);
const config11Rules = {
  minTotalRounds: normal.loadUint(8),
  maxTotalRounds: normal.loadUint(8),
  minWins: normal.loadUint(8),
  maxLosses: normal.loadUint(8),
  minStoreSeconds: normal.loadUint(32),
  maxStoreSeconds: normal.loadUint(32),
  bitPriceNanoPerSecond: normal.loadUintBig(32).toString(),
  cellPriceNanoPerSecond: normal.loadUintBig(32).toString(),
};
normal.endParse();
assert.deepEqual(config11Rules, {
  minTotalRounds: live.config11.minTotalRounds,
  maxTotalRounds: live.config11.maxTotalRounds,
  minWins: live.config11.minWins,
  maxLosses: live.config11.maxLosses,
  minStoreSeconds: live.config11.minStoreSeconds,
  maxStoreSeconds: live.config11.maxStoreSeconds,
  bitPriceNanoPerSecond: live.config11.bitPriceNanoPerSecond,
  cellPriceNanoPerSecond: live.config11.cellPriceNanoPerSecond,
});

const validator = config34.beginParse();
assert.equal(validator.loadUint(8), 0x12);
const validatorSince = validator.loadUint(32);
const validatorUntil = validator.loadUint(32);
const validatorCount = validator.loadUint(16);
const mainValidators = validator.loadUint(16);
const totalWeight = validator.loadUintBig(64);
const validatorDictionary = validator.loadDict(Dictionary.Keys.Uint(16), inlineCellValue);
validator.endParse();
let summedWeight = 0n;
for (const [index, descriptorCell] of validatorDictionary) {
  const descriptor = descriptorCell.beginParse();
  const tag = descriptor.loadUint(8);
  assert(tag === 0x53 || tag === 0x73);
  assert.equal(descriptor.loadUint(32), 0x8e81278a);
  const publicKeyHex = hex256(descriptor.loadUintBig(256));
  const weight = descriptor.loadUintBig(64);
  const adnlAddressHex = tag === 0x73 ? hex256(descriptor.loadUintBig(256)) : null;
  descriptor.endParse();
  summedWeight += weight;
  const entry = ledger.entries[index];
  assert.equal(entry.index, index);
  assert.equal(entry.publicKeyHex, publicKeyHex);
  assert.equal(entry.adnlAddressHex, adnlAddressHex);
  assert.equal(entry.weight, weight.toString());
  assert.equal(entry.offChainEvidence, null);
  assert.equal(entry.countedSupportWeight, "0");
}
const thresholdFloor = totalWeight * 3n / 4n;
const requiredYesWeight = thresholdFloor + 1n;
assert.equal(validatorSince, live.config34.utimeSince);
assert.equal(validatorUntil, live.config34.utimeUntil);
assert.equal(validatorCount, live.config34.totalValidators);
assert.equal(mainValidators, live.config34.mainValidators);
assert.equal(validatorDictionary.size, validatorCount);
assert.equal(summedWeight, totalWeight);
assert.equal(totalWeight.toString(), live.config34.totalWeight);
assert.equal(thresholdFloor.toString(), live.config34.thresholdFloor);
assert.equal(requiredYesWeight.toString(), live.config34.requiredYesWeight);
assert.equal(ledger.summary.attributableEvidenceEntries, 0);
assert.equal(ledger.summary.verifiedOffChainSupportWeight, "0");
assert.equal(ledger.summary.actualOnChainVoteWeight, "0");
assert.equal(ledger.summary.readiness, "NO_GO");

// Official TL-B order regression: rounds_remaining -> wins -> losses.
const counterRegression = beginCell().storeUint(3, 8).storeUint(0, 8).storeUint(3, 8).endCell().beginParse();
const counters = {
  roundsRemaining: counterRegression.loadUint(8),
  wins: counterRegression.loadUint(8),
  losses: counterRegression.loadUint(8),
};
counterRegression.endParse();
assert.deepEqual(counters, { roundsRemaining: 3, wins: 0, losses: 3 });
assert.notDeepEqual(counters, { roundsRemaining: 3, wins: 3, losses: 0 });

assert.equal(live.historicalOccurrence.preservedTerminalState, "REJECTED");
assert.equal(live.safetyState.hardStop, true);
assert.equal(live.safetyState.specialGovernanceStatusVerified, false);
assert.equal(live.safetyState.submissionReadiness, "NO_GO");
assert.equal(live.safetyState.realWalletAccesses, 0);
assert.equal(live.safetyState.signatures, 0);
assert.equal(live.safetyState.broadcasts, 0);
assert.equal(live.safetyState.newProposalOccurrences, 0);

process.stdout.write(`${JSON.stringify({
  result: "VERIFIED_PUBLIC_REVIEW_PACKAGE_NO_GO",
  masterchainBlock: live.masterchainBlock,
  officialConfigContract: configAddress,
  configContractSeqno,
  proposalCellId: proposalId,
  config31BeforeHash: beforeHash,
  config31AfterHash: afterHash,
  added: [candidate],
  removed: [],
  changedConfigParameters: [31],
  proposalActiveOnChain: false,
  counterOrder: "rounds_remaining -> wins -> losses",
  config11: config11Rules,
  config34: {
    hash: hash(config34),
    totalWeight: totalWeight.toString(),
    requiredYesWeight: requiredYesWeight.toString(),
  },
  config36: "ABSENT_UNAVAILABLE_NOT_COUNTED",
  verifiedSupportWeight: "0",
  requiredRemainingWeight: requiredYesWeight.toString(),
  historicalOccurrence: "REJECTED",
  readiness: "NO_GO",
  hardStop: true,
  walletAccesses: 0,
  signatures: 0,
  broadcasts: 0,
  newProposalOccurrences: 0,
}, null, 2)}\n`);
