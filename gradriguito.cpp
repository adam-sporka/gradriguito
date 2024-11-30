#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

using TRuleChar = char;
const int MAX_DEPTH = 32;

class CRules
{
public:
	std::map<TRuleChar, std::string> m_Boxes;

	CRules()
	{
		m_Boxes['A'] = "0123";
		m_Boxes['B'] = "<AA>";
		m_Boxes['C'] = "{AA}";
		m_Boxes['D'] = "";
		m_Boxes['E'] = "";
		m_Boxes['F'] = "";
		m_Boxes['G'] = "";
		m_Boxes['H'] = "";
		m_Boxes['I'] = "";
		m_Boxes['J'] = "";
		m_Boxes['K'] = "";
		m_Boxes['L'] = "";
	}

	static bool isNonTerminal(TRuleChar x)
	{
		if (x >= 'A' && x <= 'L') return true;
		return false;
	}

	static bool isTerminal(TRuleChar x)
	{
		return !isNonTerminal(x); 
	};
};

class CInstance
{
public:
	std::string        m_InitSeq;
	CRules&            m_Rules;
	std::vector<int>   m_PosVec;

	const int          BEGIN = -1;
	const int          END = 9999;

	CInstance(CRules &rules) : m_Rules(rules) {};

	void start(const std::string& init_seq)
	{
		m_InitSeq = init_seq;
		m_PosVec = { BEGIN };
	}

#ifdef _DEBUG
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
#endif

	void advancePos(bool &out_end_reached, bool &output_produced, TRuleChar &out_rule_char)
	{
		std::string seq = m_InitSeq;

		out_end_reached = false;
		output_produced = false;
		out_rule_char = '.';

		// Resolve BEGIN at the last pos
		if (m_PosVec.back() == BEGIN)
		{
			for (auto p = m_PosVec.begin(); p != m_PosVec.end() && (p + 1) != m_PosVec.end(); p++)
			{
				seq = m_Rules.m_Boxes[seq[*p]];
			}

			if (seq.length() == 0)
			{
				m_PosVec.pop_back();
				m_PosVec.push_back(END);
				return;
			}
			else
			{
				m_PosVec.pop_back();
				m_PosVec.push_back(0);
				if (CRules::isNonTerminal(seq[0]))
				{
					m_PosVec.push_back(BEGIN);
				}
				return;
			}
		}

		// Resolve END at the last pos
		if (m_PosVec.back() == END)
		{
			if (m_PosVec.size() == 1)
			{
				out_end_reached = true;
				return;
			}
			else
			{
				m_PosVec.pop_back();
				for (auto p = m_PosVec.begin(); p != m_PosVec.end() && (p + 1) != m_PosVec.end(); p++)
				{
					seq = m_Rules.m_Boxes[seq[*p]];
				}

				auto above = m_PosVec.back();
				above++;
				if (above >= seq.length())
				{
					above = END;
				}
				m_PosVec.pop_back();
				m_PosVec.push_back(above);
				if (above != END)
				{
					if (CRules::isNonTerminal(seq[above]))
					{
						m_PosVec.push_back(BEGIN);
					}
				}
				return;
			}
		}

		for (auto p = m_PosVec.begin(); p != m_PosVec.end() && (p + 1) != m_PosVec.end(); p++)
		{
			seq = m_Rules.m_Boxes[seq[*p]];		
		}

		if (m_PosVec.back() >= seq.length())
		{
			m_PosVec.pop_back();
			m_PosVec.push_back(END);
			return;
		}

		int index = m_PosVec.back();
		if (CRules::isNonTerminal(seq[index]))
		{
			m_PosVec.pop_back();
			index++;
			m_PosVec.push_back(index);
			m_PosVec.push_back(BEGIN);
		}
		else
		{
			out_rule_char = seq[index];
			output_produced = true;
			m_PosVec.pop_back();
			index++;
			m_PosVec.push_back(index);
		}
	}
};

CRules rules;

void debug_traversal(const char *seq)
{
	CInstance instance(rules);
	instance.start(seq);

	bool end_reached;
	bool output_produced;
	TRuleChar rule_char;

	do
	{
		printf("%-16s", instance.getPosAsString().c_str());

		instance.advancePos(end_reached, output_produced, rule_char);
		if (!output_produced)
		{
			printf("again          ");
		}
		else
		{
			printf("%c              ", rule_char);
		}

		printf("%-16s ", instance.getPosAsString().c_str());
		printf("\n");

	} while (!end_reached);
}

void simple_traversal(const char* seq)
{
	CInstance instance(rules);
	instance.start(seq);

	bool end_reached;
	bool output_produced;
	TRuleChar rule_char;

	do
	{
		instance.advancePos(end_reached, output_produced, rule_char);
		if (output_produced)
		{
			printf("%c", rule_char);
		}
	} while (!end_reached);
}

int main()
{
	printf("Gradriguito\n");
	debug_traversal("C");
	return 0;
}
