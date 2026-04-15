#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

// Adding sys_exit Functionality
void sys_exit(int status)
{
  struct thread *cur = thread_current();
  cur->exit_status = status;
  thread_exit();
}

// Verify uaddr is correct before accessing
static int copy_in_u32(const void *uaddr)
{
  if (uaddr == NULL || !is_user_vaddr(uaddr))
    sys_exit(-1);

  void *kpage = pagedir_get_page(thread_current()->pagedir, uaddr);
  if (kpage == NULL)
    sys_exit(-1);

  return *(int *)uaddr;
}

// Validate user range for writes
static void validate_user_range(const void *uaddr, unsigned size)
{
  const uint8_t *ptr = uaddr;

  for (unsigned i = 0; i < size; i++)
  {
    if (ptr == NULL || !is_user_vaddr(ptr) ||
      pagedir_get_page(thread_current()->pagedir, ptr) == NULL)
    {
      sys_exit(-1);
    }
    ptr++;
  }
}

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  //printf ("system call!\n");

  // Replaced exisiting systemcall handler to call sys_exit 
  // Added SYS_Halt behavior
  // Copy_in_u32 verify 
  int syscall_no = copy_in_u32(f->esp);
  switch (syscall_no)
  {
    case SYS_EXIT:
    {
      int status = *(int *)(f->esp + 4);
      sys_exit(status);
      break;
    }

    case SYS_HALT:
    {
      shutdown_power_off();
      break;
    }

    default:
    {
      sys_exit(-1);
      break;
    }
  }
}
