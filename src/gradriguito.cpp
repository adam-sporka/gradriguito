#include "3rd-party/AudioFile.h"

#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <fstream>

using TRuleStep = char;

////////////////////////////////////////////////////////////////
class CRules
{
public:
	std::map<TRuleStep, std::string> m_Boxes;

	////////////////////////////////
	CRules()
	{
		m_Boxes['A'] = "????????????????????????????????";
		m_Boxes['B'] = "________________________________";
		m_Boxes['C'] = "__--__________------------------";
		m_Boxes['D'] = "______------______------______--";
		m_Boxes['E'] = "BBBBBBBBBBBBBBBB";
		m_Boxes['F'] = "CCCCCCCCCCCCCCCC";
		m_Boxes['G'] = "DDDDDDDDDDDDDDDD";
		m_Boxes['H'] = "AAFEFEFEEEAAGEGEGEEE";
		m_Boxes['I'] = "AAFEFEFEGEAAEEFEEEEE";
		m_Boxes['J'] = "AAEEEEEEEEAAEEEEEEEE";
		m_Boxes['K'] = "AAEEAAEEAAEEAAEEAAEEAAEEAAEEAAEE";
		m_Boxes['L'] = "JJJKHIHIHHHIHIHIHHHIJJ";
	}

	////////////////////////////////
	bool parseFile(const char* path)
	{
		try
		{
			m_Boxes.clear();

			std::ifstream file(path);
			std::string str;
			while (std::getline(file, str))
			{
				if (str[0] == '#') continue;
				char first_char = '\0';
				for (auto c : str) {
					if (first_char == '\0') {
						if (isNonTerminal(c)) {
							first_char = c;
						}
					}
					else {
						if (isNonTerminal(c) || isTerminal(c))
						{
							m_Boxes[first_char].push_back(c);
						}
					}
				}
			}
		}
		catch (...)
		{
			printf("Exception while reading %s\n", path);
			return false;
		}
	}

	////////////////////////////////
	static bool isNonTerminal(TRuleStep x)
	{
		if (x >= 'A' && x <= 'Z') return true;
		return false;
	}

	////////////////////////////////
	static bool isTerminal(TRuleStep x)
	{
		if (x == '_' || x == '-' || x == '0' || x == '?') return true;
		return false;
	};
};

////////////////////////////////////////////////////////////////
class CInstance
{
public:
	std::string        m_InitSeq;
	CRules&            m_Rules;
	std::vector<int>   m_PosVec;

	const int          BEGIN = -1;
	const int          END = 9999;

	CInstance(CRules &rules) : m_Rules(rules) {};

	////////////////////////////////
	void start(const std::string& init_seq)
	{
		m_InitSeq = init_seq;
		m_PosVec = { BEGIN };
	}

	////////////////////////////////
	std::string getPosAsString()
	{
		std::string out = "";
		char temp[16];
		for (auto p : m_PosVec)
		{
			if (!out.empty()) out = out + ".";

			if (p == BEGIN) out = out + "BEGIN";
			else if (p == END) out = out + "END";
			else 
			{
				_itoa_s(p, temp, 16, 10);
				out = out + temp;
			}
		}
		return out;
	}

	////////////////////////////////
	bool endReached()
	{
		if (m_PosVec.size() == 1)
		{
			if (m_PosVec.back() == END)
			{
				return true;
			}
		}
		return false;
	}

	////////////////////////////////
	void getLeastSignificantSequence(std::string** seq)
	{
		for (auto p = m_PosVec.begin(); p != m_PosVec.end() && (p + 1) != m_PosVec.end(); p++)
		{
			*seq = &m_Rules.m_Boxes[(**seq)[*p]];
		}
	}

	////////////////////////////////
	void setLeastSignificantIndex(int index)
	{
		m_PosVec.pop_back();
		m_PosVec.push_back(index);
	}

	////////////////////////////////
	bool handleNonTerminalSymbol(TRuleStep& chr)
	{
		if (CRules::isNonTerminal(chr))
		{
			m_PosVec.push_back(BEGIN);
			return true;
		}
		return false;
	}

	////////////////////////////////
	void advancePos(bool &output_produced, TRuleStep &out_rule_char)
	{
		std::string* seq = &m_InitSeq;

		output_produced = false;
		out_rule_char = '\0';

		// Resolve BEGIN at the last pos
		if (m_PosVec.back() == BEGIN)
		{
			getLeastSignificantSequence(&seq);
			if (seq->length() == 0)
			{
				setLeastSignificantIndex(END);
				return;
			}
			else
			{
				setLeastSignificantIndex(0);
				handleNonTerminalSymbol((*seq)[0]);
				return;
			}
		}

		// Resolve END at the last pos
		if (m_PosVec.back() == END)
		{
			if (m_PosVec.size() == 1)
			{
				return;
			}
			else
			{
				m_PosVec.pop_back();
				getLeastSignificantSequence(&seq);
				auto index = m_PosVec.back();
				index++;
				if (index >= seq->length())
				{
					index = END;
				}
				setLeastSignificantIndex(index);
				if (index != END)
				{
					handleNonTerminalSymbol((*seq)[index]);
				}
				return;
			}
		}

		// Resolve a regular index
		getLeastSignificantSequence(&seq);
		if (m_PosVec.back() >= (*seq).length())
		{
			setLeastSignificantIndex(END);
			return;
		}
		int index = m_PosVec.back();
		if (!handleNonTerminalSymbol((*seq)[index]))
		{
			out_rule_char = (*seq)[index];
			output_produced = true;
			index++;
			setLeastSignificantIndex(index);
		}
	}
};

////////////////////////////////////////////////////////////////
void debug_traversal(CRules rules, const char *seq)
{
	CInstance instance(rules);
	instance.start(seq);

	bool output_produced;
	TRuleStep rule_char;

	do
	{
		printf("%-16s", instance.getPosAsString().c_str());
		instance.advancePos(output_produced, rule_char);
		if (!output_produced)
		{
			printf("               ");
		}
		else
		{
			printf("%c              ", rule_char);
		}
		printf("%-16s ", instance.getPosAsString().c_str());
		printf("\n");

	} while (!instance.endReached());
}

////////////////////////////////////////////////////////////////
int get_count_samples(CRules rules, const char* seq)
{
	CInstance instance(rules);
	instance.start(seq);

	int o = 0;
	bool output_produced;
	TRuleStep rule_char;

	do
	{
		instance.advancePos(output_produced, rule_char);
		if (output_produced)
		{
			o++;
		}
	} while (!instance.endReached());

	return o;
}

////////////////////////////////////////////////////////////////
void create_wav_file(CRules &rules, const char* filename, const char* seq)
{
	int allocated_length = 32000;

	AudioFile<char> a;
	a.setNumChannels(1);
	a.setSampleRate(8000);
	a.setBitDepth(8);
	a.setNumSamplesPerChannel(allocated_length);

	CInstance instance(rules);
	instance.start(seq);

	int pos = 0;
	bool output_produced;
	TRuleStep rule_char;

	do
	{
		instance.advancePos(output_produced, rule_char);
		if (output_produced)
		{
			switch (rule_char)
			{
				case '_': a.samples[0][pos] = -16; break;
				case '-': a.samples[0][pos] = 16; break;
				case '0': a.samples[0][pos] = 0; break;
				case '?': a.samples[0][pos] = rand() % 33 - 16; break;
				default: a.samples[0][pos] = 0;
			}
			pos++;

			if (pos >= allocated_length)
			{
				allocated_length += 32000;
				a.setNumSamplesPerChannel(allocated_length);
			}
		}
	} while (!instance.endReached());

	a.setNumSamplesPerChannel(pos);
	a.save(filename);
}

////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	if (argc < 4)
	{
		printf("Usage:\n");
		printf("gradriguito <rule_file> <output.wav> <start_rule>\n");
		printf("e.g.: gradriguito beat.gto beat.wav L\n");
		return 0;
	}

	std::string rule_file = argv[1];
	std::string output_file = argv[2];
	std::string start_seq = argv[3];

	CRules rules;
	if (!rules.parseFile(rule_file.c_str()))
	{
		printf("Error parsing %s\n", rule_file.c_str());
		return 1;
	}

	create_wav_file(rules, output_file.c_str(), start_seq.c_str());
	return 0;
}
