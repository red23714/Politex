#include "bi-derevo.h"

int main()
{
    bi_tree bi;
    init_bi_tree(&bi);

    insert_bi_tree(&bi, 6, 13);
    insert_bi_tree(&bi, 8, 12);
    insert_bi_tree(&bi, 7, 14); 
    insert_bi_tree(&bi, 4, 15);

    printf("%d\n", search_bi_tree(&bi, 7));

    printf("printing\n");

    print_bi_tree(&bi);

    printf("deleting\n");

    delete_bi_tree(&bi);

    printf("copying\n");

    copy_bi_tree(&bi); 

    return 0;
}
