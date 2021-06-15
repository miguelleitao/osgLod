# osgLod
Build an osg::LOD node from a set of external object models. 

## Usage
       ./osgLod [options] { ModelSpec1 ModelSpec2 .... } [max_dist] 
           options: -o OutputFile          : Save result to OutputFile
                    -g                     : Build an OSG::Group, without LOD
                    -p                     : Build an OSG::PagedLOD
                    -y                     : Build an OSG::Layer
                    -b                     : Build an OSG::BillBoard
                    -c min_dist            : Use min_dist as closest view distance
                    -s scale_dist          : Use scale_dist to increase range distances
           ModelSpec: [ min_dist ] ModelFilename

ModelSpecs must be listed from closest for farther.

If OutputFile is not specified, the produced model is shown in an interactive viewer.
