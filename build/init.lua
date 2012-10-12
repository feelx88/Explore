function Explore.init()
    log( "Running Init function" )
    server = Explore.Instance:getExploreServer()
    ip = "127.0.0.1"
    port = 6556
    quit, q = os.exit, os.exit
end

function createItem( name )
        return ItemFactory.create( Explore.Instance, Explore.WorldPlayer, name )
end

function deleteItem( item )
        Explore.WorldPlayer:removeOwnedItem( item )
end
