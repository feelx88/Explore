from ExploreBind import *

def LOG( what ):
    if isinstance( what, list ):
        for x in what:
            LoggerSingleton.instance().log( str( x ) )
    else:
        LoggerSingleton.instance().log( str( what ) )

LOG( "== Importing init module ==" )
#server = Explore.Instance.getExploreServer()
#messenger = server.getNetworkMessenger()

ip = "127.0.0.1"
port = 6556
q = exit
