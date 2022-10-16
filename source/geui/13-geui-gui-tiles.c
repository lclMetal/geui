void updateGuiTileIndices(TileIndices *indices, long newIndex);
void eraseGuiTiles(TileIndices *indices);
void colorGuiTiles(TileIndices indices, Color color);

void updateGuiTileIndices(TileIndices *indices, long newIndex)
{
    updateIndexBounds(&indices->first, &indices->last, newIndex);
}

void eraseGuiTiles(TileIndices *indices)
{
    destroyClones("a_gui", indices->first, indices->last);
    *indices = noIndices;
}

void colorGuiTiles(TileIndices indices, Color color)
{
    colorClones("a_gui", indices.first, indices.last, color);
}
