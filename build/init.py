from ExploreBind import *
import sys

def LOG( what ):
    if isinstance( what, list ):
        for x in what:
            LoggerSingleton.instance().log( str( x ) )
    else:
        LoggerSingleton.instance().log( str( what ) )

if __name__ == "__main__":
    LOG( "== Importing init module ==" )

    args = StringVector()
    args.extend( sys.argv )
    Explore.Instance = Explore( args )

    Explore.Instance.run()
