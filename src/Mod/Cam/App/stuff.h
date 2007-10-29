
//Funktor für sort von pair<float,TopoDS_Compound> damit die Pairs von groß nach klein sortiert werden
bool FloatWirePairHuge(const std::pair<float,TopoDS_Wire>& _Left, const std::pair<float,TopoDS_Wire>& _Right)
{
	return _Left.first > _Right.first;
}

bool FloatHuge(const float _Left, const float _Right)
{
	return _Left > _Right;
}

struct edge_container
{
	TopoDS_Edge edge;
	gp_Pnt firstPoint;
	gp_Pnt lastPoint;

};

