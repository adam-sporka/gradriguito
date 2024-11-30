#include "3rd-party/AudioFile.h"

#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

using TRuleStep = char;

////////////////////////////////////////////////////////////////
class CRules
{
public:
	std::map<TRuleStep, std::string> m_Boxes;

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

	static bool isNonTerminal(TRuleStep x)
	{
		if (x >= 'A' && x <= 'L') return true;
		return false;
	}

	static bool isTerminal(TRuleStep x)
	{
		return !isNonTerminal(x); 
	};
};

CRules rules;

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
void debug_traversal(const char *seq)
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
void simple_traversal(const char* seq)
{
	CInstance instance(rules);
	instance.start(seq);

	bool output_produced;
	TRuleStep rule_char;

	do
	{
		instance.advancePos(output_produced, rule_char);
		if (output_produced)
		{
			printf("%c", rule_char);
		}
	} while (!instance.endReached());
}

////////////////////////////////////////////////////////////////
int get_count_samples(const char* seq)
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
void create_wav_file(const char* seq, const char* filename)
{
	int length = get_count_samples(seq);

	AudioFile<char> a;
	a.setNumChannels(1);
	a.setSampleRate(8000);
	a.setBitDepth(8);
	a.setNumSamplesPerChannel(length);

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
				case '?': a.samples[0][pos] = rand() % 33 - 16; break;
				default: a.samples[0][pos] = 0;
			}
			pos++;
		}
	} while (!instance.endReached());

	a.save(filename);
}

////////////////////////////////////////////////////////////////
int main()
{
	create_wav_file("L", "L3.wav");
	return 0;
}
