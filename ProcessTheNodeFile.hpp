#ifndef PROCESSNODE_HPP
#define PROCESSNODE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

struct ProcessHppNodeInput{
    std::string title; //"Color"
    std::string element;//"Color picker or range slide bar"
    std::string type; //vec3 , vec2 , float 
};

std::vector<std::vector<std::string>> lists;

struct ProcessHppNodeOutput{
    std::string title; //"Color"
    std::string type; //vec3 , vec2 , float 
};
struct ProcessHppNode{
    std::vector<ProcessHppNodeInput> inputs;
    std::vector<ProcessHppNodeOutput> outputs;
    std::string title;
    float color[4] = {20.f,20.f,20.f,100.f};
    
    std::string code;
};

class processNode
{
public:

ProcessHppNode processNodeFile(std::string filePath){
    
    
    std::fstream filein;
    filein.open(filePath);

    if(!filein.is_open()){
        std::cout << "Unable to open the file";
    }

    std::string completeFile;

    std::string codeFile;

    std::string completeToken;
    int i = 0;
    int stateChanged = 0;
    std::string line;
    while(std::getline(filein,line))
    {


        completeFile += line;
        line = removeComments(line);
        
        if(line[0] == '%'){ //Major tokens
            completeToken = processTheWord(line,1,true,';');
            
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
        }
        if(code && i != stateChanged){
            codeFile += line + '\n';
        }
        if((inputDefinitions || outputDefinitions)){ //Subsubtokens
            processSubsubtoken(line);
        }



        i++;
    }
    createTheCode(codeFile);
    return processHppNode;
}

private:

    bool attributes = false;
    bool code = false;

    bool inputDefinitions = false;
    bool listDefinitions = false;
    bool outputDefinitions = false;


    bool uniformDefinitions = false;




    ProcessHppNode processHppNode;

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
        "list_",
        "output_",
        "title",
        "color",
        "uniforms",
        "opacity",
        "",
        ""
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
    int currentListIndex;
    int currentOutputIndex;

    std::string processTheWord(std::string line, int startingIndex, bool processSpaces, char haltingChar){
        int i = startingIndex;

        std::string word;

        if(processSpaces){
            while(line[i] != ' ' && line[i] != haltingChar){
                word += line[i];
                i++;
            }
        }
        else{
            while(line[i] != haltingChar){
                word += line[i];
                i++;
            }
        }

        return word;
    }


    //----------------------------------PROCESS THE COLORS----------------------------------
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

    float* hsvToRgbConverter(float h,float s,float v){
	    float r,g,b;

        unsigned char region, remainder, p, q, t;

        if (v == 0)
        {
            r = v;
            g = v;
            b = v;
            float result[3];

            result[0] = r;
            result[1] = g;
            result[2] = b;
            return result;
        }

        region = v / 43;
        remainder = (v - (region * 43)) * 6; 

        //ERROR : expression must have integral or uscoped enum type
        p = (v * (255 - v)) >> 8;
        q = (v * (255 - ((v * remainder) >> 8))) >> 8;
        t = (v * (255 - ((v * (255 - remainder)) >> 8))) >> 8;

        switch (region)
        {
            case 0:
                r = v; g = t; b = p;
                break;
            case 1:
                r = q; g = v; b = p;
                break;
            case 2:
                r = p; g = v; b = t;
                break;
            case 3:
                r = p; g = q; b = v;
                break;
            case 4:
                r = t; g = p; b = v;
                break;
            default:
                r = v; g = p; b = q;
                break;
        }

        float result[3];

        result[0] = r;
        result[1] = g;
        result[2] = b;
        return result;
    }
    void processTheColors(std::string line, std::string attribute,int attributeIndex){
        //Process the color type (hex , rgb , hsv)
        std::string colorType;
        for(int i = 0; i < 4; i++){
            colorType += attribute[i];
        }
        //Process the color value
        
        float* value;
        if(colorType == "hex"){
            //Process the hex value
            std::string colorValue = processTheWord(line,attributeIndex + 3,false,';'); 
            value = hexToRGBConverter('#' + colorValue);
        }
        if(colorType == "rgb"){//rgb153,45,112
            //Process the rgb value
            std::string valueR = processTheWord(line,attributeIndex + 3,false,',');
            std::string valueG = processTheWord(line,attributeIndex + 4 + valueR.size(),false,',');
            std::string valueB = processTheWord(line,attributeIndex + 5 + valueR.size() + valueG.size(),false,';');
            value[0] = std::stoi(valueR);
            value[1] = std::stoi(valueG);
            value[2] = std::stoi(valueB);
        }
        if(colorType == "hsv"){
            //Process the hsv value
            std::string valueR = processTheWord(line,attributeIndex + 3,false,',');
            std::string valueG = processTheWord(line,attributeIndex + 4 + valueR.size(),false,',');
            std::string valueB = processTheWord(line,attributeIndex + 5 + valueR.size() + valueG.size(),false,';');
            value = hsvToRgbConverter(std::stoi(valueR),std::stoi(valueG),std::stoi(valueB));
        }
        processHppNode.color[0] = value[0];//R
        processHppNode.color[1] = value[1];//G
        processHppNode.color[2] = value[2];//B
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

        return result;
    }

    void processSubToken(std::string line){
        if(line[0] == '-' && line[1] != '-'){ //Check subtokens

            //Example 
            //title : imatitle
            //title = subtoken
            // : = required equation sign
            //imatitle = attribute


            std::string completeToken = processTheWord(line,1,true,';');
            
            std::string attribute;

            bool properToken = checkIfArrayContainsTheWord(subTokens,completeToken);

            const int signIndex = completeToken.size() + 2;

            bool tokenIsInputOrOutputOrList = false;
            for (size_t i = 0; i < 60; i++)
            {
                if(completeToken == "input_" + std::to_string(i)){
                    tokenIsInputOrOutputOrList = true;
                    break;
                }
                if(completeToken == "output_" + std::to_string(i)){
                   tokenIsInputOrOutputOrList = true;
                    break;
                }
                if(completeToken == "list_" + std::to_string(i)){
                   tokenIsInputOrOutputOrList = true;
                    break;
                }
            }


            if(properToken || tokenIsInputOrOutputOrList){
                if(line[signIndex] == ':'){
                    if(!tokenIsInputOrOutputOrList)

                    //Process the index for the attribute
                    int attributeIndex = 0;
                    //attributeIndex not defined
                    while(line[signIndex + attributeIndex + 1] != ' '){
                        attributeIndex++;
                    }
                    attributeIndex = signIndex + attributeIndex + 1;

                    //Attribute will contain spaces if the token is title
                    if(completeToken == "title"){
                        attribute = processTheWord(line,attributeIndex,true,';');
                        processHppNode.title = attribute;
                    }
                    else{
                        attribute = processTheWord(line,attributeIndex,false,';');
                    }



                    if(completeToken == "color"){
                        processTheColors( line, attribute,attributeIndex);
                    }



                    if(completeToken == "opacity"){
                        processHppNode.color[3] = std::stoi(attribute);
                    }   




                    const int maxInputCount = 60;
                    for (size_t i = 0; i < 60; i++)
                    {
                        if(completeToken == "input_" + std::to_string(i)){
                            if(processHppNode.inputs.size() < i){
                                std::cout << "ERROR : Wrong index : " << completeToken << std::endl;
                            }
                            else{
                                ProcessHppNodeInput input;
                                //Default input values
                                input.element = "none";
                                input.title = completeToken;
                                input.type = "float";
                                processHppNode.inputs.push_back(input);

                                inputDefinitions = true;
                                listDefinitions = false;
                                outputDefinitions = false;

                                currentInputIndex = i;
                            }
                        }
                        if(completeToken == "list_" + std::to_string(i)){
                            if(processHppNode.inputs.size() < i){
                                std::cout << "ERROR : Wrong index : " << completeToken << std::endl;
                            }
                            else{
                                std::vector<std::string> list;
                                //Default input values
                                list.push_back("list_" + std::to_string(i));
                                lists.push_back(list);

                                listDefinitions = true;
                                inputDefinitions = false;
                                outputDefinitions = false;

                                currentInputIndex = i;
                            }
                        }

                        if(completeToken == "output_" + std::to_string(i)){
                            if(processHppNode.outputs.size() < i){
                                std::cout << "ERROR : Wrong index : " << completeToken << std::endl;
                            }
                            else{
                                ProcessHppNodeOutput output;
                                //Default output values
                                output.title = completeToken;
                                output.type = "float";
                                processHppNode.outputs.push_back(output);

                                outputDefinitions = true;
                                inputDefinitions = false;
                                listDefinitions = false;


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

        while (i < attribute.size())
        {
            if(attribute[i] == '|'){
                uniforms.push_back(currentWord);
                currentWord = "";
                i++;
            }
            currentWord += attribute[i];
            i++;
        }
        uniforms.push_back(currentWord);
        currentWord = "";
    }

    void processSubsubtoken(std::string line){
        if(line[0] == '-' && line[1] == '-'){
                std::string completeToken = processTheWord(line,2,true,';');
                
                //Process the token
                bool titleToken = false;
                bool typeToken = false;
                bool elementToken = false;

                if(completeToken == "title"){
                    titleToken = true;
                }
                if(completeToken == "type"){
                    typeToken = true;
                }
                if(completeToken == "element"){
                    elementToken = true;
                }
                if((!titleToken && !typeToken && !elementToken) || outputDefinitions && elementToken){
                    std::cout << "ERROR : Invalid Token : " << completeToken << std::endl;
                }

                else{
                    bool goOn = false;

                    //
                    int signIndex = completeToken.size()+2;
                    if(line[signIndex] == ':'){
                        goOn = true;
                    }
                    
                    //Process the index for the attribute
                    int attributeIndex = 0;
                    while(line[signIndex + attributeIndex + 1] != ' '){
                        attributeIndex++;
                    }
                    attributeIndex = signIndex + attributeIndex + 1;


                    if(goOn){
                        std::string attribute;
                        if(titleToken){
                            attribute = processTheWord(line,attributeIndex,false,';');
                        }
                        else{
                            attribute = processTheWord(line,attributeIndex,true,';');
                        }

                        if(titleToken){
                            if(inputDefinitions)
                                processHppNode.inputs[currentInputIndex].title = attribute;
                            if(outputDefinitions)
                                processHppNode.outputs[currentOutputIndex].title = attribute;
                        }
                        if(typeToken){
                            if(inputDefinitions)
                                processHppNode.inputs[currentInputIndex].type = attribute;
                            if(outputDefinitions)
                                processHppNode.outputs[currentOutputIndex].type = attribute;
                        }
                        if(elementToken){
                            if(inputDefinitions)
                                processHppNode.inputs[currentInputIndex].element = attribute;
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
        int commentIndex = 30000;
        for (size_t i = 0; i < line.size(); i++)
        {
            if(line[i] == commentChar){
                commentIndex = i;
            }
        }
        if(commentIndex != 30000)
            line.erase(commentIndex,line.size()-1);

        return line;
    }

    void createTheCode(std::string code){
        std::string version = "#version 330 core \n";

        std::string result;

        result.append(version);

        for (size_t i = 0; i < uniforms.size(); i++)
        {
            std::string type;

            std::string in = "in " + uniformData[uniforms[i]] + ' ' + uniforms[i] + ';' + '\n';
            result.append(in);
        }
        
        for (size_t i = 0; i < processHppNode.inputs.size(); i++)
        {
            std::string uniform = "uniform " + processHppNode.inputs[i].type + " input_" + std::to_string(i)+ ';' + '\n';
            result.append(uniform);
        }
        for (size_t i = 0; i < processHppNode.outputs.size(); i++)
        {
            std::string out = "layout(location=" + std::to_string(i) + ") out " + processHppNode.outputs[i].type + " output_" + std::to_string(i) + ';' + '\n';
            result.append(out);
        }
        
        result.append(code);

        if(processHppNode.outputs.size() == 0){
            std::cout << "WARNING : This node has no outputs" << std::endl;
        }

        processHppNode.code = result;
    }
};

#endif