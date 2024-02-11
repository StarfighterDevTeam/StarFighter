#include "Individual.h"
#include "rapidxml-1.13/RapidXmlLib.h"
#include <fstream>
#include <sstream>

Individual::Individual()
{
	m_generation = nullptr;
	m_badGene = -1;
	randomizeDNA();

	//evolution record
	//Evolution* evolution = new Evolution();
	//evolution->m_gen = 0;
	//Individual::copyDNA(m_dna, evolution->m_dna_output);
	//m_evolution_record.push_back(evolution);
}

Individual::~Individual()
{
	//evolution record
	//for (int i = 0; i < m_evolution_record.size(); i++)
	//{
	//	delete m_evolution_record[i];
	//}
	//m_evolution_record.clear();
}

void Individual::randomizeDNA()
{
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		m_dna[i] = RandomizeIntBetweenValues(0, DNA_ENTROPY - 1);
	}
}

void Individual::copyDNA(const int dna_input[], int dna_output[])
{
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		dna_output[i] = dna_input[i];
	}
}

void Individual::DisplayDNA()
{
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		printf("%d", m_dna[i]);
		if (i < DNA_LENGTH - 1)
			printf(",");
	}
};

void Individual::DisplayDNA(int dna[])
{
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		printf("%d", dna[i]);
		if (i < DNA_LENGTH - 1)
			printf(",");
	}
};

void Individual::DisplayEvolutionRecord(int dna_secret[])
{
	printf("Evolution record:\n");
	for (int i = 0; i < m_evolution_record.size(); i++)
	{
		Evolution& evo = *m_evolution_record[i];
		
		if (evo.m_gen > -1)
		{
			printf("-Generation: %d", evo.m_gen);
			printf(" -> ");
			DisplayDNA();
			printf(", Fitness: %d", ComputeFitness(evo.m_dna_output, dna_secret));
			printf("\n");
		}
		
		if (evo.m_mutation > -1)
		{
			printf("	-Mutation: ");
			switch (evo.m_mutation)
			{
				case Mutation_Add:
				{
					printf("Add.");
					break;
				}
				case Mutation_Erase:
				{
					printf("Erase.");
					break;
				}
				case Mutation_Expand:
				{
					printf("Expand.");
					break;
				}
			}

			printf(" ");
			DisplayDNA(evo.m_dna_input_a);
			printf(" -> ");
			DisplayDNA(evo.m_dna_output);
			printf(", Fitness: %d", ComputeFitness(evo.m_dna_output, dna_secret));
			printf("\n");
		}

		if (evo.m_crossover > -1)
		{
			printf("	-Crossover: ");
			switch (evo.m_crossover)
			{
				case CrossOver_FirstHalf:
				{
					printf("First half.");
					break;
				}
				case CrossOver_SecondHalf:
				{
					printf("Second half.");
					break;
				}
				case CrossOver_AlternateOdd:
				{
					printf("Alternate odd.");
					break;
				}
				case CrossOver_AlternateNotOdd:
				{
					printf("Alternate not odd.");
					break;
				}
			}

			printf(" ");
			DisplayDNA(evo.m_dna_input_a);
			printf(" + ");
			DisplayDNA(evo.m_dna_input_b);
			printf(" -> ");
			DisplayDNA(evo.m_dna_output);
			printf(", Fitness: %d", ComputeFitness(evo.m_dna_output, dna_secret));
			printf("\n");
		}
	}
};

void Individual::copy(const Individual& individual)
{
	m_index = individual.m_index;
	m_fitness = individual.m_fitness;
	m_gen = individual.m_gen;
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		m_dna[i] = individual.m_dna[i];
	}

	//evolution record
	//for (int i = 0; i < m_evolution_record.size(); i++)
	//{
	//	delete m_evolution_record[i];
	//}
	//m_evolution_record.clear();
	//for (int i = 0; i < individual.m_evolution_record.size(); i++)
	//{
	//	Evolution* evolution = new Evolution();
	//	evolution->copy(*individual.m_evolution_record[i]);
	//	m_evolution_record.push_back(evolution);
	//}
}

void Individual::crossOver(Individual& output, const Individual& input_a, const Individual& input_b, CrossOverType type)
{
	output.copy(input_a);

	switch (type)
	{
		case CrossOver_FirstHalf:
		{
			for (int i = 0; i < DNA_LENGTH; i++)
			{
				if (i < DNA_LENGTH / 2)
					output.m_dna[i] = input_b.m_dna[i];
				else
					output.m_dna[i] = input_a.m_dna[i];
			}
			break;
		}
		case CrossOver_SecondHalf:
		{
			for (int i = 0; i < DNA_LENGTH; i++)
			{
				if (i < DNA_LENGTH / 2)
					output.m_dna[i] = input_a.m_dna[i];
				else
					output.m_dna[i] = input_b.m_dna[i];
			}
			break;
		}
		case CrossOver_AlternateOdd:
		{
			for (int i = 0; i < DNA_LENGTH; i++)
			{
				if (i % DNA_LENGTH == 0)
					output.m_dna[i] = input_a.m_dna[i];
				else
					output.m_dna[i] = input_b.m_dna[i];
			}
			break;
		}
		case CrossOver_AlternateNotOdd:
		{
			for (int i = 0; i < DNA_LENGTH; i++)
			{
				if (i % 2 == 0)
					output.m_dna[i] = input_b.m_dna[i];
				else
					output.m_dna[i] = input_a.m_dna[i];
			}
			break;
		}
		default:
			break;
	}

	//evolution record
	Evolution* evolution = new Evolution();
	evolution->m_crossover = (int)type;
	Individual::copyDNA(input_a.m_dna, evolution->m_dna_input_a);
	Individual::copyDNA(input_b.m_dna, evolution->m_dna_input_b);
	Individual::copyDNA(output.m_dna, evolution->m_dna_output);
	output.m_evolution_record.push_back(evolution);
	
}

void Individual::mutate(Individual& output, const Individual& input, MutationType type, int repeat)
{
	output.copy(input);

	for (int i = 0; i < repeat; i++)
	{
		const int r = (i == 0 && input.m_badGene >= 0) ? input.m_badGene : RandomizeIntBetweenValues(0, DNA_LENGTH - 1);//prioritize mutation of a bad gene if any
		const int v = RandomizeIntBetweenValues(1, DNA_ENTROPY - 1);

		switch (type)
		{
			case Mutation_Add:
			{
				output.m_dna[r] = (input.m_dna[r] + v) % DNA_ENTROPY;
				break;
			}
			case Mutation_Erase:
			{
				output.m_dna[r] = 0;
				break;
			}
			case Mutation_Expand:
			{
				output.m_dna[(r + v) % DNA_ENTROPY] = input.m_dna[r];
				break;
			}
			default:
				break;
		}
	}

	//evolution record
	//Evolution* evolution = new Evolution();
	//evolution->m_mutation = (int)type;
	//Individual::copyDNA(input.m_dna, evolution->m_dna_input_a);
	//Individual::copyDNA(output.m_dna, evolution->m_dna_output);
	//output.m_evolution_record.push_back(evolution);
}

int Individual::ComputeFitness(int dna_individual[], int dna_secret[])
{
	std::vector<int> slots_marked;

	int fitness = 0;
	for (int i = 0; i < DNA_LENGTH; i++)//proposal
	{
		if (dna_individual[i] == dna_secret[i])
		{
			//"red" marker
			fitness += 10;
			slots_marked.push_back(i);
			continue;
		}
	}

	for (int i = 0; i < DNA_LENGTH; i++)//proposal
	{
		for (int j = 0; j < DNA_LENGTH; j++)//secret
		{
			if (dna_individual[i] == dna_secret[j])
			{
				bool found = false;
				for (int v = 0; v < slots_marked.size(); v++)
				{
					if (slots_marked[v] == j)
					{
						found = true;
						break;
					}
				}

				if (found == false)
				{
					//"white" marker
					fitness += 4;
					slots_marked.push_back(i);
					continue;
				}
			}
		}
	}

	return fitness;
}

int Individual::ComputeFitness(Individual& individual, Individual& const secret)
{
	individual.m_fitness = ComputeFitness(individual.m_dna, secret.m_dna);

	return individual.m_fitness;
}

bool Individual::saveInFile()
{
	using namespace rapidxml;
	xml_document<> doc;

	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	xml_node<>* individualNode = doc.allocate_node(node_element, "individual");
	doc.append_node(individualNode);

	appendXmlAttribute(doc, individualNode, "fitness", m_fitness);

	for (int i = 0; i < DNA_LENGTH - 1; i++)
	{
		xml_node<>* geneNode = doc.allocate_node(node_element, "gene");
		appendXmlAttribute(doc, geneNode, "state", i);
		appendXmlAttribute(doc, geneNode, "action", m_dna[i]);
		individualNode->append_node(geneNode);
	}
		
	// Save to file
	const std::string path = HERO_XML_FILE;
	std::ofstream outFile(path);
	if (!outFile.is_open())
	{
		return false;
	}
	outFile << doc;
	outFile.close();
	doc.clear();

	return true;
}

bool Individual::loadFromFile()
{
	using namespace rapidxml;
	xml_document<> doc;

	std::ifstream file(HERO_XML_FILE);
	if (!file.is_open())
		return false;

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content(buffer.str());
	doc.parse<0>(&content[0]);

	xml_node<>* individualNode = doc.first_node("individual");
	if (!individualNode)
		return false;

	setFitness(0);
	setFitness(std::stoi(individualNode->first_attribute("fitness")->value()));

	int geneId = 0;
	for (xml_node<>* geneNode = individualNode->first_node("gene"); geneNode; geneNode = geneNode->next_sibling("gene"))
	{
		m_dna[geneId] = std::stoi(geneNode->first_attribute("action")->value());
		geneId++;
	}

	return true;
}