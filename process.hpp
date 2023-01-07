#ifndef PROCESSNODE_HPP
#define PROCESSNODE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

class processNode
{
public:

std::string processNodeFile(std::string filePath){
    std::ifstream filein(filePath);

    std::string completeFile;

    std::string codeFile;

    std::string completeToken;
    int i = 0;
    int stateChanged = 0;
    for (std::string line; std::getline(filein, line);) 
    {
        completeFile += line;
        line = removeComments(line);
        
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
        if(code && i != stateChanged){
            codeFile += line;
        }

        if((inputDefinitions || outputDefinitions) && i != stateChanged){ //Subsubtokens
            processSubsubtoken(line);
        }



        i++;
    }

    createTheCode(codeFile);
}

private:

    bool attributes = false;
    bool code = false;

    bool inputDefinitions = false;
    bool outputDefinitions = false;


    bool uniformDefinitions = false;

    struct NodeInput{
        std::string title; //"Color"
        std::string element;//"Color picker or range slide bar"
        std::string type; //Sampler2D, vec3 , vec2 , float 
    };
    struct NodeOutput{
        std::string title; //"Color"
        std::string type; //Sampler2D, vec3 , vec2 , float 
    };
    struct Node{
        std::vector<NodeInput> inputs;
        std::vector<NodeOutput> outputs;
        std::string title;

        float color[4] = {20.f,20.f,20.f,100.f};
    };

    Node node;

    struct Code{
        std::vector<std::string> uniforms;
        std::string code;
    };


    std::vector<std::string> inputTokens = {
        "title",
        "element",
        "type"
    };
    std::vector<std::string> outputTokens = {
        "title",
        "type"
    };
    std::vector<std::string> subTokens = {
        "input_",
        "output_",
        "title",
        "color",
        "uniforms"
        "opacity"
    };
    std::vector<std::string> tokens = {
        "attributes",
        "code"
    };

    std::map<std::string,std::string> uniformData{
        {"tex_coords","vec2"}, {"normal","vec3"}, {"pos","vec3"}
    };

    std::vector<std::string> uniforms;

    int currentInputIndex;
    int currentOutputIndex;

    std::string processTheWord(std::string line, int startingIndex){
        int i = startingIndex;

        std::string word;

        while(line[i] != ' ' && line[i] != ';'){
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


    bool checkIfArrayContainsTheWord(std::vector<std::string> array, std::string word){
        bool result;

        std::vector<std::string>::iterator foo;
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
                    attribute = processTheWord(line,signIndex+2);

                    if(completeToken == "title"){
                        node.title = attribute.c_str();
                    }
                    if(completeToken == "color"){
                        float* value = hexToRGBConverter(attribute); 
                        node.color[0] = value[0];//R
                        node.color[1] = value[1];//G
                        node.color[2] = value[2];//B
                    }
                    if(completeToken == "opacity"){
                        node.color[3] = std::stoi(attribute);
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
                                outputDefinitions = false;

                                currentInputIndex = i;
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
                                inputDefinitions = false;

                                currentOutputIndex = i;
                            }
                        }
                    }
                    if(completeToken == "uniforms"){
                        interpretTheUniforms(attribute);
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

    void interpretTheUniforms(std::string attribute){
        
        //TODO : Check if uniform is correct

        std::string currentWord;
        int i = 0;

        if(attribute[attribute.size()-1] == ';'){
            while (attribute[i] != ';')
            {
                if(attribute[i] == '|'){
                    uniforms.push_back(currentWord.c_str());
                    currentWord = "";
                    i++;
                }
                currentWord += attribute[i];
                i++;
            }
            uniforms.push_back(currentWord.c_str());
            currentWord = "";
        }
        else{
            std::cout << "ERROR : ';' Expected : " << attribute << std::endl;
        }
    }

    void processSubsubtoken(std::string line){
        if(line[0] == '-' && line[1] == '-'){
                std::string completeToken = processTheWord(line,2);
                
                std::string title = "title";
                std::string type = "type";
                std::string element = "element";
                
                bool titleToken = true;
                bool typeToken = true;
                bool elementToken = true;

                for (size_t i = 0; i < 5; i++)
                {
                    if(completeToken[i] == title[i]){
                        
                    }
                    else{
                        titleToken = false;
                        break;
                    }
                }
                for (size_t i = 0; i < 4; i++)
                {
                    if(completeToken[i] == type[i]){
                        
                    }
                    else{
                        typeToken = false;
                        break;
                    }
                }
                for (size_t i = 0; i < 7; i++)
                {
                    if(completeToken[i] == element[i]){
                        
                    }
                    else{
                        elementToken = false;
                        break;
                    }
                }

                if((!titleToken && !typeToken && !elementToken) || outputDefinitions && elementToken){
                    std::cout << "ERROR : Invalid Token : " << completeToken << std::endl;
                }
                else{
                    bool goOn = false;

                    int index = 0;

                    if(titleToken){
                        if(line[8] == ':'){
                            goOn = true;
                            index = 8+2;
                        }
                    }
                    if(typeToken){
                        if(line[7] == ':'){
                            goOn = true;
                            index = 7+2;
                        }
                    }
                    if(elementToken){
                        if(line[10] == ':'){
                            goOn = true;
                            index = 10+2;
                        }
                    }

                    if(goOn){
                        std::string attribute;
                        attribute = processTheWord(line,index);

                        if(titleToken){
                            if(inputDefinitions)
                                node.inputs[currentInputIndex].title = attribute.c_str();
                            if(outputDefinitions)
                                node.outputs[currentOutputIndex].title = attribute.c_str();
                        }
                        if(typeToken){
                            if(inputDefinitions)
                                node.inputs[currentInputIndex].type = attribute.c_str();
                            if(outputDefinitions)
                                node.outputs[currentOutputIndex].type = attribute.c_str();
                        }
                        if(elementToken){
                            if(inputDefinitions)
                                node.inputs[currentInputIndex].element = attribute.c_str();
                        }

                    }
                    else{
                        std::cout << "ERROR : ':' Expected " << line << std::endl;
                    }
                }
            }
    }

    std::string removeComments(std::string line){
        const char commentChar = '#';
        int commentIndex;
        for (size_t i = 0; i < line.size(); i++)
        {
            if(line[i] == commentChar){
                commentIndex = i;
            }
        }
        line.erase(commentIndex,line.size()-1);

        return line;
    }

    std::string createTheCode(std::string code){
        std::string version = "#version 330 core \n";

        std::string result;

        result.append(version);

        for (size_t i = 0; i < uniforms.size(); i++)
        {
            std::string type;

            std::string in = "in " + uniformData[uniforms[i]] + ' ' + uniforms[i] + '\n';
        }
        
        for (size_t i = 0; i < node.inputs.size(); i++)
        {
            std::string uniform = "uniform " + node.inputs[i].type + " input_" + std::to_string(i) + '\n';
            result.append(uniform);
        }
        for (size_t i = 0; i < node.outputs.size(); i++)
        {
            std::string out = "layout(location=" + std::to_string(i) + ") out " + node.outputs[i].type + " output_" + std::to_string(i) + '\n';
            result.append(out);
        }
        
        result.append(code);

        if(node.outputs.size() == 0){
            std::cout << "WARNING : This node has no outputs" << std::endl;
        }

    }
};

#endif