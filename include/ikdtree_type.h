#ifndef IKDTREE_TYPE_H_
#define IKDTREE_TYPE_H_


typedef	struct
{
	unsigned short	max;
	unsigned short	min;
	int	child[2];
}ikdtree_node;

typedef	struct
{
	int			number_of_nodes;
	int			buffer_size;
	int			min_dim;
	int			min_diff;
	int			max_depth;
	ikdtree_node*		node;
}ikdtree;



#endif /* IKDTREE_TYPE_H_ */
