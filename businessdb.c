#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Customer {
  char *email;
  char *name;
  int shoe_size;
  char *fav_food;
  struct Customer *next;
} Customer;

// Credit to Dan Bernstein
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while (*str != '\0') {
    c = *str;
    hash = ((hash << 5) + hash) + (unsigned char)c; /* hash * 33 + c */
    str++;
  }
  return hash;
}

Customer *addtoList(char *email, char *name, int shoe_size, char *fav_food,
                    Customer *bucket) {
  struct Customer *newList;

  newList = malloc(sizeof(Customer));
  newList->email = strdup(email);
  newList->name = strdup(name);
  newList->shoe_size = shoe_size;
  newList->fav_food = strdup(fav_food);
  newList->next = bucket;
  return newList;
}

void addtoHash(char *email, char *name, int shoe_size, char *fav_food,
               Customer **buckets, size_t num_buckets) {
  size_t which_bucket = hash(email) % num_buckets;
  buckets[which_bucket] =
      addtoList(email, name, shoe_size, fav_food, buckets[which_bucket]);
  // printf("friend %d goes in bucket %lu .\n", name, which_bucket);
}

bool validEmail(char *email) {
  int atCount = 0;
  int dotCount = 0;
  for (int i = 0; i < (int)strlen(email); i++) {
    if (email[i] == '@') {
      atCount++;
    }
    if (email[i] == '.') {
      dotCount++;
    }
  }
  if (atCount == 1 && dotCount == 1) {
    return true;
  } else {
    return false;
  }
}

bool validName(char *name) {
  for (int i = 0; i < (int)strlen(name) - 1; i++) {
    if ((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z') || (name[i] == ' ')) {
      continue;
    } else {
      return false;
    }
  }
  for(int j = 0; j < (int)strlen(name) - 1;j++) {
	  if(name[j] != ' ') {
		  return true;
	  }
  }

  return false;
}

bool validNumber(char *num) {
	for(int i = 0; i < (int)strlen(num)-1; i++) {
		if(num[i] >= '0' && num[i] <= '9') {
		       continue;
	      	}
		else {
			return false;
		}
	}
	return true;
}

int main(void) {
  size_t NUM_BUCKETS;
  size_t customeramt = 0;
  FILE *customers = fopen("customers.tsv", "r");
  char *line = NULL;
  size_t len = 0;
  if (customers == NULL) {
    printf("Failed to open file");
    return 0;
  }
  for(char c = getc(customers); c != EOF; c = getc(customers)) {
	  if (c == '\n') {
		  customeramt++;
	  }
  }
  fclose(customers);
  customers = fopen("customers.tsv", "r");
  NUM_BUCKETS = customeramt * 2;
  if (NUM_BUCKETS == 0) {
	  printf("No customers were read.\n");
	  return 0;
  }
  Customer *buckets[NUM_BUCKETS];
  for (size_t i = 0; i < NUM_BUCKETS; i++) {
	  buckets[i] = NULL;
  }
  while (getline(&line, &len, customers) != -1) {
    char *email = strtok(line, "\t");
    char *name = strtok(NULL, "\t");
    int shoe_size = atoi(strtok(NULL, "\t"));
    char *fav_food = strtok(NULL, "\t");
    fav_food[strcspn(fav_food, "\n")] = 0; 
    addtoHash(email, name, shoe_size, fav_food, buckets, NUM_BUCKETS);
  }
  free(line);
  fclose(customers);
  while (true) {
    char command[100];
    printf("Command: ");
    fgets(command, 100, stdin);
    command[(int)strlen(command)-1] = '\0';
    if (strcmp(command, "quit") == 0) {
      for (size_t i = 0; i < NUM_BUCKETS; i++) {
        if (buckets[i] != NULL) {
          Customer *user = buckets[i];
          while (user != NULL) {
            Customer *next = user->next;
            free(user->email);
            user->email = NULL;
            free(user->name);
            user->name = NULL;
            free(user->fav_food);
            user->fav_food = NULL;
            free(user);
            user = next;
          }
        }
      }
      break;
    } else if (strcmp(command, "save") == 0) {
      FILE *save = fopen("customer.tsv", "w");
      if (save == NULL) {
        printf("Could not create output file...\n");
      }
      for (size_t i = 0; i < NUM_BUCKETS; i++) {
        if (buckets[i] != NULL) {
          fprintf(save, "%s\t%s\t%d\t%s\n", buckets[i]->email, buckets[i]->name,
                  buckets[i]->shoe_size, buckets[i]->fav_food);
        }
      }
      fclose(save);
      printf("File successfully saved!\n");

    } else if (strcmp(command, "add") == 0) { 
      char *email = malloc(256);
      char *name = malloc(256);
      char *shoe_size_buf = malloc(256);
      char *fav_food = malloc(256);
      printf("Email?: ");
      while (validEmail(email) == false) {
        char *get = fgets(email, 255, stdin);
        if ((int)get == '\n') {
          continue;
        }
        if (!validEmail(email)) {
          printf("Email?: ");
        }
      }
      email[strcspn(email, "\n")] = 0;
      printf("Name?: ");
      while (true) {
        char *get = fgets(name, 255, stdin);
	if ((int)get == '\n') {
          printf("Name?: ");
          continue;
        }
        if (validName(name)) {
          break;
        } else {
          printf("Name?: ");
          continue;
        }
      }
      name[strcspn(name, "\n")] = 0;
      int shoe_size;
      printf("Shoe size?: ");
      while (true) {
	char *get = fgets(shoe_size_buf, 255, stdin);
	if ((int)get == '\n') {
		printf("Shoe size?: ");
		continue;
        }
        if(validNumber(shoe_size_buf)) {
		break;
	}
	else {
		printf("Shoe size?: ");
		continue;
	}
      }
      shoe_size_buf[strcspn(shoe_size_buf, "\n")] = 0;
      shoe_size = atoi(shoe_size_buf);
      printf("Favorite Food?: ");
      //fgets(fav_food, 255, stdin);
      while (true) {
        char *get = fgets(fav_food, 255, stdin);
	if ((int)get == '\n') {
          printf("Favorite Food?: ");
          continue;
        }
	if (validName(fav_food)) {
          break;
        } else {
          printf("Favorite Food?: ");
          continue;
        }
      }
      fav_food[strcspn(fav_food, "\n")] = 0;
      if (buckets[hash(email) % NUM_BUCKETS] != NULL &&
          strcmp(email, buckets[hash(email) % NUM_BUCKETS]->email) == 0) {
        free(buckets[hash(email) % NUM_BUCKETS]->name);
	free(buckets[hash(email) % NUM_BUCKETS]->fav_food);
	buckets[hash(email) % NUM_BUCKETS]->name = name;
        buckets[hash(email) % NUM_BUCKETS]->shoe_size = shoe_size;
        buckets[hash(email) % NUM_BUCKETS]->fav_food = fav_food;
        free(email);
	free(shoe_size_buf);
	customeramt++;
	printf("Customer already exists! Information is now changed.\n");
      } else {
        // printf("%lu\n %lu", hash("bimmy@ucsc.edu") % NUM_BUCKETS, hash(email)
        // % NUM_BUCKETS);
        addtoHash(email, name, shoe_size, fav_food, buckets, NUM_BUCKETS);
        customeramt++;
	free(email);
	free(name);
	free(shoe_size_buf);
	free(fav_food);
	printf("Successfully added the customer.\n");
      } 
    } else if (strcmp(command, "lookup") == 0) {
      char inputemail;
      printf("Email?: ");
      scanf("%255s", &inputemail);
      while (validEmail(&inputemail) == false) {
        printf("Not a valid email!\n");
        printf("Email?: ");
        scanf("%255s", &inputemail);
      }
      printf("\n");
      if (buckets[hash(&inputemail) % NUM_BUCKETS] != NULL &&
          strcmp(buckets[hash(&inputemail) % NUM_BUCKETS]->email,
                 &inputemail) == 0) {
        Customer *user = buckets[hash(&inputemail) % NUM_BUCKETS];
        printf("Email: %s\nName: %s\nShoe size: %d\nFavorite food: %s\n",
               user->email, user->name, user->shoe_size, user->fav_food);
      } else {
        printf("User doesn't exist with that email!\n");
      }
    } else if (strcmp(command, "delete") == 0) {
      char inputemail;
      printf("Email?: ");
      scanf("%255s", &inputemail);
      while (validEmail(&inputemail) == false) {
        printf("Not a valid email!\n");
        printf("Email?: ");
        scanf("%255s", &inputemail);
      }
      Customer *user = buckets[hash(&inputemail) % NUM_BUCKETS];
      if (user == NULL) {
        printf("User with that email does not exist!\n");
      } else {
        free(buckets[hash(&inputemail) % NUM_BUCKETS]->email);
        free(buckets[hash(&inputemail) % NUM_BUCKETS]->name);
        free(buckets[hash(&inputemail) % NUM_BUCKETS]->fav_food);
        buckets[hash(&inputemail) % NUM_BUCKETS]->email = NULL;
        buckets[hash(&inputemail) % NUM_BUCKETS]->name = NULL;
        buckets[hash(&inputemail) % NUM_BUCKETS]->fav_food = NULL;
        user = user->next;
        free(buckets[hash(&inputemail) % NUM_BUCKETS]);
        buckets[hash(&inputemail) % NUM_BUCKETS] = NULL;
        printf("Successfully deleted the customer!\n");
        customeramt--;
      }
    } else if (strcmp(command, "list") == 0) {
      printf("\n");
      for (size_t i = 0; i < NUM_BUCKETS; i++) {
        if (buckets[i] != NULL) {
          Customer *user = buckets[i];
          printf("Email: %s\nName: %s\nShoe size: %d\nFavorite food: %s\n",
                 user->email, user->name, user->shoe_size, user->fav_food);
        }
      }
      printf("\n");
    } else {
      printf("Unknown Command\n");
    }
  }
}
