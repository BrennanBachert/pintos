#include "userprog/syscall.h"
#include <stdbool.h>
#include <stdio.h>
#include <syscall-nr.h>
#include "devices/shutdown.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
// due to comp error added this
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

//Pre-Declaring so dont have to worry about ordering
static int syscall_write (int fd, const void *buffer, unsigned size);
static void sys_exit (int status) NO_RETURN;
static int get_user (const uint8_t *uaddr);
static bool copy_in (void *dst_, const void *usrc_, size_t size);
static uint32_t copy_in_u32 (const void *uaddr);
static void validate_user_range (const void *uaddr, unsigned size);
static void syscall_handler (struct intr_frame *);

static int
syscall_write (int fd, const void *buffer, unsigned size)
{
  if (fd != 1)
    return -1;

  putbuf(buffer, size);
  return size;
}

// Adding sys_exit Functionality
static void sys_exit(int status)
{
  struct thread *cur = thread_current();
  cur->exit_status = status;
  thread_exit();
}

// Returns byte at user virtual address UADDR / -1 on segfault
static int get_user (const uint8_t *uaddr) {
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:" : "=&a" (result) : "m" (*uaddr));
  return result;
}

// Copies SIZE bytes from user address USRC_ into kernel buffer DST_
// Returns true on success, false if any byte is invalid. 
static bool
copy_in (void *dst_, const void *usrc_, size_t size)
{
  uint8_t *dst = dst_;
  const uint8_t *usrc = usrc_;

  for (size_t i = 0; i < size; i++)
    {
      if (usrc == NULL || !is_user_vaddr (usrc))
        return false;
      if (pagedir_get_page (thread_current()->pagedir, usrc) == NULL)
        return false;
      int byte = get_user (usrc);
      if (byte < 0)
        return false;
      dst[i] = (uint8_t) byte;
      usrc++;
    }
  return true;
}

static uint32_t
copy_in_u32 (const void *uaddr) {
  uint32_t value;
  if (!copy_in (&value, uaddr, sizeof value))
    sys_exit (-1);
  return value;
}

// Ensure [UADDR, UADDR + SIZE) is readable by user
static void
validate_user_range (const void *uaddr, unsigned size)
{
  const uint8_t *ptr = uaddr;
  for (unsigned i = 0; i < size; i++, ptr++)
  {
    if (ptr == NULL || !is_user_vaddr (ptr) ||
        pagedir_get_page (thread_current()->pagedir, ptr) == NULL ||
        get_user (ptr) < 0)
      sys_exit (-1);
  }
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  // Replaced exisiting systemcall handler to call sys_exit 
  // Added SYS_Halt behavior
  // Copy_in_u32 verify 
  // SYS_WRITE Implement
  int syscall_no = (int) copy_in_u32 (f->esp);
  switch (syscall_no)
  {
    case SYS_EXIT:
    {
      int status = (int)copy_in_u32(f->esp + 4);
      sys_exit (status);
      break;
    }

    case SYS_HALT:
    {
      shutdown_power_off();
      break;
    }
    //Functionality for writing that should verify everything
    case SYS_WRITE:
    {
      //getting everything needed for syscall_write
      int fd = copy_in_u32(f->esp + 4);
      const void *buffer = (const void *) copy_in_u32(f->esp + 8);
      unsigned size = (unsigned) copy_in_u32(f->esp + 12);

      validate_user_range(buffer, size);
      f->eax = syscall_write(fd, buffer, size);
      break;
    }

    default:
    {
      sys_exit(-1);
      break;
    }
  }
}
