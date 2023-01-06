#ifndef PROCESSNODE_HPP
#define PROCESSNODE_HPP


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class processNode
{
public:

std::string processNodeFile(const char* filePath){
    std::ifstream filein(filePath);

    std::string completeFile;

    std::string codeFile;

    std::string completeToken;

    int i = 0;
    int stateChanged = 0;
    for (std::string line; std::getline(filein, line);) 
    {
        completeFile += line;
        
        if(line[0] == '$'){ //Major tokens
            completeToken = processTheWord(line,1);
            if(completeToken == "attributes"){
                attributes = true;
                code = false;
                stateChanged = i;
            }
            else if(completeToken == "code"){
                attributes = false;
                code = true;
                stateChanged = i;
            }
            else{
                std::cout << "ERROR : Invalid Preindicator : " << completeToken << std::endl;
                break;
            }
        }

        if(attributes && i != stateChanged){ //Subtokens
            processSubToken(line);
            stateChanged = i;
        }

        if((inputDefinitions || outputDefinitions) && i != stateChanged){ //Subsubtokens

        }

        if(code && i != stateChanged){
            codeFile += line;
        }

        i++;
    }
}

private:

    bool attributes = false;
bool code = false;

bool inputDefinitions = false;
bool outputDefinitions = false;


bool uniformDefinitions = false;

struct NodeInput{
    const char * title; //"Color"
    const char * element;//"Color picker or range slide bar"
    const char * type; //Sampler2D, vec3 , vec2 , float 
};
struct NodeOutput{
    const char * title; //"Color"
    const char * type; //Sampler2D, vec3 , vec2 , float 
};
struct Node{
    std::vector<NodeInput> inputs;
    std::vector<NodeOutput> outputs;
    const char* title;
    
    float color[4] = {20.f,20.f,20.f,100.f};
};

Node node;

struct Code{
    std::vector<const char *> uniforms;
    const char * code;
};

std::vector<const char *> inputTokens = {
    "title",
    "element",
    "type"
};
std::vector<const char *> subTokens = {
    "input_",
    "output_",
    "title",
    "color",
    "uniforms"
};
std::vector<const char *> tokens = {
    "attributes",
    "code"
};

std::string processTheWord(std::string line, int startingIndex){
    int i = startingIndex;
    
    std::string word;

    while(line[i] != ' ' || !std::isdigit(line[i])){
        word += line[i];
        i++;
    }

    return word;
}

float* hexToRGBConverter(std::string hex){ //takes hex : #000000 (# is required) 
	float r;
	float g;
	float b;
	int hexVal[6];
	for (int i = 1; i < 7; i++)
	{
		if(hex[i] == '0'){hexVal[i-1] = 0;}
		if(hex[i] == '1'){hexVal[i-1] = 1;}
		if(hex[i] == '2'){hexVal[i-1] = 2;}
		if(hex[i] == '3'){hexVal[i-1] = 3;}
		if(hex[i] == '4'){hexVal[i-1] = 4;}
		if(hex[i] == '5'){hexVal[i-1] = 5;}
		if(hex[i] == '6'){hexVal[i-1] = 6;}
		if(hex[i] == '7'){hexVal[i-1] = 7;}
		if(hex[i] == '8'){hexVal[i-1] = 8;}
		if(hex[i] == '9'){hexVal[i-1] = 9;}
		if(hex[i] == 'a'){hexVal[i-1] = 10;}
		if(hex[i] == 'b'){hexVal[i-1] = 11;}
		if(hex[i] == 'c'){hexVal[i-1] = 12;}
		if(hex[i] == 'd'){hexVal[i-1] = 13;}
		if(hex[i] == 'e'){hexVal[i-1] = 14;}
		if(hex[i] == 'f'){hexVal[i-1] = 15;}
	}
	
	r = hexVal[0] * 16 + hexVal[1];
	g = hexVal[2] * 16 + hexVal[3];
	b = hexVal[4] * 16 + hexVal[5];
	float result[3] = {r,g,b};
	return result;
}


bool checkIfArrayContainsTheWord(std::vector<const char *> array, const char * word){
    bool result;

    std::vector<const char *>::iterator foo;
    foo = std::find(std::begin(array), std::end(array), word);
    if (foo != std::end(array)) {
        result = true;
    } 
    else {
        result = false;
    }
}

void processSubToken(std::string line){
    if(line[0] == '-' && line[1] != '-'){ //Check subtokens
        std::string completeToken = processTheWord(line,1);
        std::string attribute;

        bool properToken = checkIfArrayContainsTheWord(subTokens,completeToken.c_str());

        const int signIndex = completeToken.size() + 2;

        if(properToken){
            if(line[signIndex] == ':'){
                attribute = processTheWord(line,signIndex+1);
                
                if(completeToken == "title"){
                    node.title = attribute.c_str();
                }
                if(completeToken == "color"){
                    float* value = hexToRGBConverter(attribute); 
                    node.color[0] = value[0];//R
                    node.color[1] = value[1];//G
                    node.color[2] = value[2];//B
                }
                const int maxInputCount = 60;
                for (size_t i = 0; i < 60; i++)
                {
                    if(completeToken == "input_" + std::to_string(i)){
                        if(node.inputs.size() < i){
                            std::cout << "ERROR : Wrong index : " << completeToken << std::endl;
                        }
                        else{
                            NodeInput input;
                            //Default input values
                            input.element = "none";
                            input.title = completeToken.c_str();
                            input.type = "float";
                            node.inputs.push_back(input);

                            inputDefinitions = true;
                        }
                    }

                    if(completeToken == "output_" + std::to_string(i)){
                        if(node.outputs.size() < i){
                            std::cout << "ERROR : Wrong index : " << completeToken << std::endl;
                        }
                        else{
                            NodeOutput output;
                            //Default output values
                            output.title = completeToken.c_str();
                            output.type = "float";
                            node.outputs.push_back(output);

                            outputDefinitions = true;

                        }

                    }
                }
                if(completeToken == "uniforms"){
                    
                }
            }
            else{
                std::cout << "ERROR : Equation required : " << completeToken << std::endl;    
            }
        }
        else{
            std::cout << "ERROR : Invalid Token : " << completeToken << std::endl;
        }
    }
}


};

#endif