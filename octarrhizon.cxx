#include <pugixml.hpp>
#include <synarmosma/markov_chain.h>

int nbars = 0;
int initial_note = -1;
unsigned long seed = 0;
std::string output_file = "";
const std::string notes[] = {"C4","D4","E4","F4","G4","A5","B5"};

void read_parameters(const std::string& filename)
{
  std::string inote = "";
  pugi::xml_document pfile;
  pugi::xml_node global;
  std::string name,value;

  // Open the file
  if (!(pfile.load_file(filename.c_str()))) throw std::invalid_argument("Unable to parse parameter file!");

  global = pfile.child("Parameters");
  for(pugi::xml_node params = global.first_child(); params; params = params.next_sibling()) {
    name = params.name();
    value = params.first_child().value();
    // Now that we have the parameter name, see if it matches
    // any of the known parameters. If so, read in the value and
    // assign it
    if (name == "NumberBars") {
      nbars = std::stoi(value);
    }
    else if (name == "InitialNote") {
      inote = value;
    }
    else if (name == "RandomSeed") {
      seed = (unsigned) std::stol(value);
    }
    else if (name == "OutputFile") {
      output_file = value;
    }
  }

  assert(nbars > 0);
  assert(!output_file.empty());
  for(int i=0; i<7; ++i) {
    if (inote == notes[i]) {
      initial_note = i;
      break;
    }
  }
  assert(initial_note >= 0);
  if (seed == 0) seed = (unsigned) std::time(nullptr);
}

void initialize_matrix(SYNARMOSMA::Markov_Chain* S,SYNARMOSMA::Random* RND)
{
  double rho;
  std::vector<double> pi;
  std::set<int> values;

  for(int i=0; i<7; ++i) {
    pi.push_back(0.0);
  }
  values.insert(1); values.insert(0); values.insert(-1);

  // Use a set of random but somewhat sensible values for the transition matrix...
  rho = 0.0;
  pi[0] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[0];
  pi[1] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[1];
  pi[2] = 0.15 + 0.05*double(RND->irandom(values)); rho += pi[2];
  pi[3] = 1.0 - rho;
  S->set_transition(pi,0);

  rho = 0.0;
  pi[0] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[0];
  pi[1] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[1];
  pi[2] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[2];
  pi[3] = 1.0 - rho;
  S->set_transition(pi,1);

  rho = 0.0;
  pi[1] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[1];
  pi[2] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[2];
  pi[3] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[3];
  rho = 0.5*(1.0 - rho);
  pi[0] = rho; pi[4] = rho;
  S->set_transition(pi,2);

  rho = 0.0;
  pi[0] = 0.0;
  pi[3] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[3];
  pi[2] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[2];
  pi[4] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[4];
  rho = 0.5*(1.0 - rho);
  pi[1] = rho; pi[5] = rho;
  S->set_transition(pi,3);

  rho = 0.0;
  pi[1] = 0.0;
  pi[4] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[4];
  pi[3] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[3];
  pi[5] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[5];
  rho = 0.5*(1.0 - rho);
  pi[2] = rho; pi[6] = rho;
  S->set_transition(pi,4);

  rho = 0.0;
  pi[2] = 0.0;
  pi[5] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[5];
  pi[6] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[6];
  pi[4] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[4];
  pi[3] = 1.0 - rho;
  S->set_transition(pi,5);

  rho = 0.0;
  pi[6] = 0.4 + 0.1*double(RND->irandom(values)); rho += pi[6];
  pi[5] = 0.2 + 0.05*double(RND->irandom(values)); rho += pi[5];
  pi[4] = 0.15 + 0.05*double(RND->irandom(values)); rho += pi[4];
  pi[3] = 1.0 - rho;
  S->set_transition(pi,6);

  assert(S->consistent());
}

int main(int argc,char** argv)
{
  if (argc != 2) {
    std::cerr << "Usage: ./octarrhizon <parameter file>" << std::endl;
    return 1;
  }

  int i,cnote,nnote;
  std::string filename(argv[1]);
  SYNARMOSMA::Markov_Chain S(7);
  SYNARMOSMA::Random* RND = new SYNARMOSMA::Random;

  read_parameters(filename);

  RND->set_seed(seed);
  S.set_seed(seed);

  initialize_matrix(&S,RND);

  std::ofstream s(output_file);

  cnote = initial_note;
  s << notes[cnote] << " ";
  for(i=1; i<4*nbars; ++i) {
    nnote = S.get_state(cnote);
    cnote = nnote;
    s << notes[cnote] << " ";
    if ((i+1) % 4 == 0) s << std::endl;
  }

  s.close();

  delete RND;

  return 0;
}
