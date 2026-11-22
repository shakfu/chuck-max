// SPDX-License-Identifier: MIT
/* words.cpp -- see words.h. */

#include "core/words.h"

#include <algorithm>

namespace ckc {

const std::vector<std::string> &chuck_keywords() {
	static const std::vector<std::string> v = {
		"break", "case", "class", "const", "continue", "default", "do", "else", "extends", "for", "fun",
		"function", "global", "if", "implements", "interface", "new", "private", "protected", "public",
		"pure", "repeat", "return", "spork", "static", "switch", "until", "while"};
	return v;
}

/* Types, UGens, standard objects and built-in values (now, dac, me, ...). */
const std::vector<std::string> &chuck_builtins() {
	static const std::vector<std::string> v = {
		"ADSR", "BPF", "BRF", "BandedWG", "BeeThree", "BlowBotl", "BlowHole", "Bowed", "Brass", "Chorus",
		"Clarinet", "Delay", "DelayA", "DelayL", "Dyno", "Echo", "Envelope", "Event", "FMVoices",
		"FileIO", "Flute", "FullRect", "Gain", "HPF", "HalfRect", "HevyMetl", "Hid", "HidMsg", "IO",
		"Impulse", "JCRev", "KBHit", "LPF", "LiSa", "Machine", "Mandolin", "Math", "MidiIn", "MidiMsg",
		"MidiOut", "ModalBar", "Moog", "NRev", "Noise", "OnePole", "OneZero", "OscIn", "OscMsg",
		"OscOut", "PRCRev", "Pan2", "PercFlut", "Phasor", "PitShift", "PoleZero", "PulseOsc", "ResonZ",
		"Rhodey", "SawOsc", "Saxofony", "Shakers", "Shred", "SinOsc", "Sitar", "SndBuf", "SndBuf2",
		"SqrOsc", "Step", "StifKarp", "Std", "TriOsc", "TubeBell", "TwoPole", "TwoZero", "UGen",
		"VoicForm", "Wurley", "adc", "blackhole", "cherr", "chout", "complex", "dac", "day", "dur",
		"false", "float", "hour", "int", "maybe", "me", "minute", "ms", "now", "null", "pi", "polar",
		"samp", "second", "string", "super", "this", "time", "true", "vec2", "vec3", "vec4", "void",
		"week"};
	return v;
}

static bool is_head(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
static bool is_tail(char c) { return is_head(c) || (c >= '0' && c <= '9'); }

void Words::sync(const std::string &text, int revision) {
	if (mSynced && revision == mRevision)
		return;
	mSynced = true;
	mRevision = revision;
	++mScans;
	mNames.clear();
	for (const std::string &b : chuck_builtins())
		mNames[b].kind = KindFunction;
	for (const std::string &k : chuck_keywords())
		mNames[k].kind = KindKeyword;
	const size_t n = text.size();
	for (size_t i = 0; i < n;) {
		/* An identifier cannot start right after a digit or another identifier byte. */
		if (is_head(text[i]) && (i == 0 || !is_tail(text[i - 1]))) {
			size_t j = i + 1;
			while (j < n && is_tail(text[j]))
				++j;
			++mNames[text.substr(i, j - i)].freq;
			i = j;
		} else {
			++i;
		}
	}
}

int Words::count(const std::string &name) const {
	auto it = mNames.find(name);
	return it == mNames.end() ? 0 : it->second.freq;
}

std::vector<CompletionItem> Words::query(const std::string &prefix, int limit) const {
	std::vector<std::pair<const std::string *, const Entry *>> hits;
	for (const auto &kv : mNames)
		if (kv.first != prefix && kv.first.compare(0, prefix.size(), prefix) == 0)
			hits.emplace_back(&kv.first, &kv.second);
	std::sort(hits.begin(), hits.end(), [](const auto &a, const auto &b) {
		if (a.second->freq != b.second->freq)
			return a.second->freq > b.second->freq;
		if (a.first->size() != b.first->size())
			return a.first->size() < b.first->size();
		return *a.first < *b.first;
	});
	if (limit > 0 && (int)hits.size() > limit)
		hits.resize((size_t)limit);
	std::vector<CompletionItem> out;
	for (const auto &h : hits) {
		CompletionItem it;
		it.label = it.insert = *h.first;
		it.kind = h.second->kind;
		out.push_back(it);
	}
	return out;
}

} // namespace ckc
