/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

// #include <cstdint>
#include <stdint.h>
#include <stdio.h>
#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
            size_t char_offset, size_t *entry_offset_byte_rtn )
{
    /**
    * TODO: implement per description
    */
    printf("Char offset: %zu\n", char_offset);
    printf("read out offset: %d\n", buffer->out_offs);
    uint8_t cumulative_bytes = 0;
    uint8_t previous_cumulative_bytes = 0;
    uint8_t index;
    struct aesd_buffer_entry *entry;
    // AESD_CIRCULAR_BUFFER_FOREACH(entry,buffer,index){
    for(index=buffer->out_offs,entry=&buffer->entry[index%AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
        index<AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED+buffer->out_offs;
        index++,entry=&buffer->entry[index%AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED]){
      printf("Index: %d\n", index);
      printf("Entry: %s\n", entry->buffptr);
      previous_cumulative_bytes = cumulative_bytes;
      cumulative_bytes += entry->size;
      printf("Cumulative bytes: %d\n", cumulative_bytes);
      if (char_offset < cumulative_bytes){
        int byte_rtn = char_offset - previous_cumulative_bytes;
        printf("char position: %d\n", byte_rtn);
        if(char_offset > 0){
          *entry_offset_byte_rtn = byte_rtn;
        } else {
          *entry_offset_byte_rtn = 0;
        }
        printf("Returning entry: %s\n", entry->buffptr);
        return entry;
      }
    } 
  
    printf("No entry found\n");
    return NULL;
  
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description
    */
  buffer->in_offs = buffer->in_offs % 10;
  buffer->out_offs = buffer->out_offs % 10;
  printf("write in offs: %d\n", buffer->in_offs);
  printf("read out offs: %d\n", buffer->out_offs);
  printf("full: %d\n", buffer->full);
  printf("write data: %s\n", add_entry->buffptr);

  // buffer->in_offs = buffer->in_offs % 10;
  buffer->entry[buffer->in_offs] = *add_entry;
  printf("buff data: %s\n", buffer->entry[buffer->in_offs].buffptr);
  if (buffer->in_offs == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED - 1){
    // buffer->out_offs++;
    buffer->full = true;
  }
  if ((buffer->in_offs == buffer->out_offs) && buffer->full){
    buffer->out_offs++;
    buffer->full = false;
  }
  buffer->in_offs++;
  printf("write in offs: %d\n", buffer->in_offs);
  printf("read out offs: %d\n", buffer->out_offs);
  printf("------------------\n");
  // if (buffer->in_offs == AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED - 1){
  //   buffer->in_offs = 0;
  //   buffer->full = true;
  // } else {
  //   buffer->in_offs++;
  // }
  // if (buffer->in_offs == buffer->out_offs){
  //   if (buffer->full){
  //     buffer->out_offs++;
  //   }
  // }
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}
