#include <iostream>
#include <string>
#include <sstream>
#include <boost/program_options.hpp>

#include "srx_segmenter.hpp"

int main(int argc, char** argv) {
    
    namespace po = boost::program_options;
    po::options_description options("SRX sentence segmenter");

    std::string language, rules;
    std::size_t soft_limit, hard_limit;
    
    options.add_options()
      ("help,h", po::bool_switch(), "Show this help message")
      ("language,l", po::value<std::string>(&language)->default_value("en"),
       "Language in SRX file")
      ("rules,r", po::value<std::string>(&rules)->default_value("srx/rules.srx"),
       "Path to SRX file")
      ("soft_limit", po::value<std::size_t>(&soft_limit)->default_value(0),
       "Break at next space after  args  bytes, disabled by default")
      ("hard_limit", po::value<std::size_t>(&hard_limit)->default_value(0),
       "Break after  args  bytes, disabled by default")
      ("trim,t", po::bool_switch(),
       "Trim leading and trailing white spaces")
      ("consume,c", po::bool_switch(),
       "Consune whole text first then split, line-by-line otherwise")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, options), vm);
    
    if(vm["help"].as<bool>()) {
        std::cerr << options << std::endl;
        return 1;
    }
    po::notify(vm);
    
    SrxSegmenter srxSeg(language, rules, hard_limit, soft_limit);
    
    if(vm["consume"].as<bool>()) {
        std::stringstream ss;
        bool doTrim = vm["trim"].as<bool>();
        char c;
        while(std::cin.get(c))
            ss << c;
        SrxSentenceCutter srxCutter(srxSeg, ss.str(), doTrim);
        std::string frag;
        while(srxCutter >> frag)
            if(!doTrim || !frag.empty())
                std::cout << frag << std::endl;
    }
    else {
        std::string line;
        while(std::getline(std::cin, line)) {
            SrxSentenceCutter srxCutter(srxSeg, line, vm["trim"].as<bool>());
            std::string frag;
            while(srxCutter >> frag)
                std::cout << frag << std::endl;
        }
    }
    return 0;
}
