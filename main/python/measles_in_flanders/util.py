import networkx as nx

def getSusceptibleContactsGraph(logfile):
    """
        Turn logfile of contacts between susceptibles
        into a graph.
    """
    G = nx.Graph()
    with open(logfile) as f:
        for line in f:
            linedata = line.split(' ')
            if linedata[0] == '[CONT]':
                p1_id = int(linedata[1])
                p2_id = int(linedata[2])
                G.add_nodes_from([p1_id, p2_id])
                G.add_edge(p1_id, p2_id)
    return G
