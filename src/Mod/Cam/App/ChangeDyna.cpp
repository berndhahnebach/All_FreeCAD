
#include "PreCompiled.h"
#include "ChangeDyna.h"
#include <sstream>

bool ChangeDyna::Read( const std::string & _filename)
{
    // open file for reading
    std::ifstream input( _filename.c_str() );
    std::ofstream output("dyna2.str");
    if ( !input.is_open() )
    {
        std::cout << "failed to open file" << std::endl;
        return false;
    }
    if ( !ReadCurve( input, output ) )
    {
        std::cout << "failed to read curve_data" << std::endl;
        return false;
    }

    input.close();
    output.close();
    return true;
}

bool ChangeDyna::ReadCurve(std::ifstream &input,std::ofstream &output)
{
    input.seekg( std::ifstream::beg );
    std::string line,subline1;
    bool found = false;
    int current_index;
    do
    {
        std::getline( input, line );
        if (line.size() == 0) //if we have an empty line
        {
            continue;
        }
        if (found)
        {
            //If we find a new Keyword description
            if (line.size() > 3 && line.at(0) == '$' && (line.find("nid") == string::npos))
            {
                found = false;
                output << line << endl;
                continue; //go directly to the next line
            }
            else if (line.at(0) == '$')
            {
                output << line << endl;
                continue;
            }
            else // Now we change the Birth and Death-Times
            {
                std::stringstream astream1,astream2;
                astream1.precision(20);
                astream2.precision(20);
                if (line.at(9) != '0') //If we are at the first line
                {
                    //Extract the Curve-Index
                    astream1.str(line.substr(10,5));
                    astream1 >> current_index;
                    //Exchange the Death time. We need a vector of pairs (birth,death)
                    //Clear the current Stream;
                    //temp_stream << times.at(current_index)
                    //Exchange the Death time. We need a vector of pairs (birth,death)
                    //Clear the current Stream;
                    //temp_stream << times.at(current_index)
                    //temp_stream.clear();
                    //To test we set the death time to
                    double temp_ = 130.1366;
                    astream2 << temp_;
                    //Now we have to reformat the string to fit exactly 9 digits
                    try
                    {
                        ReformatStream(astream2,subline1);
                        output << line.substr(0,66) << subline1 << line.substr(75,5) << endl;
                    }
                    catch (out_of_range)
                    {
                        output << line << endl;
                    }

                    continue;
                }
                else //we are at the second line and can exchange the Birth-Time
                {
                    double temp_ = 130.106;
                    astream2 << temp_;
                    try
                    {
                        ReformatStream(astream2,subline1);
                        output << line.substr(0,31) << subline1 << endl;
                    }
                    catch (out_of_range)
                    {
                        output << line << endl;
                    }

                    continue;
                }
            }
        }
        else
        {
            std::string search("Velocity/Acceleration/");
            if (line.find(search)!=string::npos)
                found = true;
            output << line << endl;
            continue;
        }
    }
    while ( input.good());
    return true;
}


bool ChangeDyna::ReformatStream(const std::stringstream& astream, std::string& astring)
{
    astring.clear();
    std::string tempstring(astream.str());
    unsigned int found=tempstring.find('.');
    astring = tempstring.substr(0,found);
    astring += '.';
    //Now add the rest. We have only space for 9 digits in total (the '-' is already there)
    astring += tempstring.substr(found+1,9-astring.size());
    while (astring.size() < 9)
        astring += '0'; //We add '0' until we have a size of 9
    return true;
}