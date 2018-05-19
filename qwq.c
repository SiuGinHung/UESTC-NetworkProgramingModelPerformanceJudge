int n = 1;

struct ListNode* last = NULL;
struct ListNode* tmp;
while(head!=NULL)
{
    tmp = head->next;
    head->next = last;
    last = head;
    head = tmp;
}