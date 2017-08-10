/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_JACK_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_JACK_INTERFACE_HPP_INCLUDED

#include "audio_interface.hpp"
#include "export_symbols.hpp"

#include <libril/constants.hpp>

#include <boost/property_tree/ptree.hpp>

#include <memory>
#include <string>
#include <vector>

namespace visr
{
namespace audiointerfaces
{
  class JackInterface: public audiointerfaces::AudioInterface
  {
  public:
    /**
     * Structure to hold all configuration arguments for a PortAudioInterface instance.
     */
    struct Config
    {
    public:
      /** Default contructor to initialise elements to defined values. */
      //                Config()
      //                : mClientName("")
      //                , mServerName("")
      //                , mPortJSONConfig("")
      //                {}
      

      Config(std::string cliName, std::string servName, boost::property_tree::ptree portsConfig,  bool autoConnect = false);

      
      void loadPortConfigJson(boost::property_tree::ptree tree, int numCapt, int numPlay );
      void loadPortConfig(boost::optional<boost::property_tree::ptree> tree, std::string & extClient, std::vector< std::string > &portNames, std::vector< std::string > & extPortNames, int numPorts, bool & autoConn, std::string porttype);
      
      void setCapturePortNames( std::string const baseName,
                               std::size_t startIndex,
                               std::size_t endIndex );
      
      void setPlaybackPortNames( std::string const baseName,
                                std::size_t startIndex,
                                std::size_t endIndex );
      
      std::string mClientName;
      std::string mInExtClientName;
      std::string mOutExtClientName;
      bool mInAutoConnect;
      bool mOutAutoConnect;
      
      std::string mServerName;
      
      boost::property_tree::ptree mPortJSONConfig;
      
      std::vector< std::string > mCapturePortNames;
      
      std::vector< std::string > mPlaybackPortNames;
      
    };
    
    using Base = AudioInterface;
    
    explicit JackInterface(  Configuration const & baseConfig, std::string const & config);
    
    ~JackInterface( );
    
    /* virtual */ void start() override;
    
    /* virtual */ void stop() override;
    
    /*virtual*/ bool registerCallback( AudioCallback callback, void* userData ) override;
    
    /*virtual*/ bool unregisterCallback( AudioCallback audioCallback ) override;
    
    
    
    //            std::size_t mNumberOfCaptureChannels;
    //            std::size_t mNumberOfPlaybackChannels;
    //
    //            std::size_t mPeriodSize;
    //
    //            using SamplingRateType = std::size_t;
    //            SamplingRateType mSampleRate;
    
  private:
    /**
     * Private implementation class to avoid dependencies to the Portaudio library in the public interface.
     */
    class Impl;
    
    
    //            JackInterface::Config parseSpecificConf( std::string const & config );
    
    /**
     * Private implementation object according to the "pointer to implementation" (pimpl) idiom.
     */
    std::unique_ptr<Impl> mImpl;
  };
  
} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_JACK_INTERFACE_HPP_INCLUDED
