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

'''
def countImmune(sim, timestep):
    if timestep == 0:
        print("Determining pocket sizes of non-immunized individuals")
        households = {}
        schools = {}
        workplaces = {}
        communities1 = {}
        communities2 = {}

        pop = sim.GetSimulator().GetPopulation()
        for pIndex in range(pop.size()):
            person = pop[pIndex]
            if person.GetHealth().IsSusceptible():
                addPerson(households, person.GetClusterId(Id_Household), True)
                addPerson(schools, person.GetClusterId(Id_School), True)
                addPerson(workplaces, person.GetClusterId(Id_Work), True)
                addPerson(communities1, person.GetClusterId(Id_PrimaryCommunity), True)
                addPerson(communities2, person.GetClusterId(Id_SecondaryCommunity), True)
            else:
                addPerson(households, person.GetClusterId(Id_Household), False)
                addPerson(schools, person.GetClusterId(Id_School), False)
                addPerson(workplaces, person.GetClusterId(Id_Work), False)
                addPerson(communities1, person.GetClusterId(Id_PrimaryCommunity), False)
                addPerson(communities2, person.GetClusterId(Id_SecondaryCommunity), False)

        # Distribution of susceptibles by cluster type'''
        '''bins = [0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        household_results = toHist(toFractions(households), bins)
        school_results = toHist(toFractions(schools), bins)
        work_results = toHist(toFractions(workplaces), bins)
        comm1_results = toHist(toFractions(communities1), bins)
        comm2_results = toHist(toFractions(communities2), bins)

        width = 0.015

        fig, ax = plt.subplots()
        ax.bar(bins, household_results, width=width, color='red')
        ax.bar([x + width for x in bins], school_results, width, color='tan')
        ax.bar([x + (2* width) for x in bins], work_results, width, color='lime')
        ax.bar([x + (3* width) for x in bins], comm1_results, width, color='orange')
        ax.bar([x + (4* width) for x in bins], comm2_results, width, color='black')
        plt.show()'''
        '''
        # Overall distribution of susceptibles
        bins = [0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
        all_fractions = toFractions(households) + toFractions(schools) + toFractions(workplaces) + toFractions(communities1) + toFractions(communities2)
        all_results = toHist(all_fractions, bins)
        plt.ylim([0,1])
        plt.legend(['Baseline'])
        plt.bar(bins, all_results, width=0.08)
        plt.savefig('baseline')'''

'''
def toHist(fractions, bins):
    ys = [0] * len(bins)
    for frac in fractions:
        for i_bin in range(len(bins)):
            if frac <= bins[i_bin]:
                ys[i_bin] += 1
                break
    for i_y in range(len(ys)):
        ys[i_y] = ys[i_y] / len(fractions)
    return ys


def toFractions(dictionary):
    fractions = []
    for key in dictionary:
        cluster_size = dictionary[key]['susceptible'] + dictionary[key]['other']
        frac_susceptible = dictionary[key]['susceptible'] / cluster_size
        fractions.append(frac_susceptible)
    return fractions


def addPerson(dictionary, cluster_id, susceptible):
    if cluster_id != 0:
        if not (cluster_id in dictionary):
            dictionary[cluster_id] = {
                'susceptible' : 0,
                'other' : 0
            }

        if susceptible:
            dictionary[cluster_id]['susceptible'] += 1
        else:
            dictionary[cluster_id]['other'] += 1
'''
