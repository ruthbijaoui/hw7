#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grades.h"
#include "linked-list.h"

typedef struct grades grades;
typedef struct students_t students_t;
typedef struct courses_t  courses_t;

struct grades {
	struct list *student_list;
};

/*A linked list that each element in contains a student name, 
*ID number and points to his courses list*/
struct students_t {
	char *name;
	int id;
	struct list *list_courses;
};

/*A linked list of a specific student that contains
*the courses he has studied and his grades*/
struct courses_t {
	char *course_name;
	int grade;	
};

students_t* create_student_node(char *name, int id) {
	students_t *node = malloc(sizeof(students_t));
	if (node == NULL){
		free(node);
		return (NULL);
	} 
	node->name = name;
	node->id = id;
	node->list_courses = NULL;
	return node;
}

courses_t* create_courses_node(char *name, int grade) {
	courses_t *node = malloc(sizeof(struct courses_t));
	if (node == NULL){
		free(node);
		return (NULL);
	}
	node->course_name = name;
	node->grade = grade;
	return node;
}

int clone_student_node (void *node_in, void **node_out) {
	students_t *in = (struct students_t*)node_in;
	students_t **out_s = (struct students_t**)node_out;
	*out_s = (struct students_t*)malloc(sizeof(students_t));

	char *name_clone = (char*)malloc((strlen(in->name)+1)*sizeof(char));
	strcpy(name_clone, in->name);

	if(!out_s || !name_clone){
		free(out_s);
		free(name_clone);
		return -1;
	}

	(*out_s)->name = name_clone;
	(*out_s)->id = in->id;
	(*out_s)->list_courses = in->list_courses;
	free(in);
	return 0;
}

int clone_courses_node (void *node_in, void **node_out){
	courses_t *in = (struct courses_t*)node_in;
	courses_t **out_c = (struct courses_t**)node_out;
	*out_c = (struct courses_t*)malloc(sizeof(courses_t));

	char *course_name_clone = (char*)malloc((strlen(in->course_name)+1)*
		sizeof(char));
	strcpy(course_name_clone, in->course_name);

	if(!out_c || !course_name_clone) {
		free(out_c);
		free(course_name_clone);
		return -1;
	}

	(*out_c)->course_name = course_name_clone;
	(*out_c)->grade = in->grade;
	free(in);
	return 0;
}

void destroy_student_node(void *node) {
	students_t *destroy_node = (struct students_t*)node;
	free(destroy_node->name);
	list_destroy(destroy_node->list_courses);
	free(destroy_node);
	return;
}

void destroy_courses_node(void *node) {
	courses_t *destroy_node = (struct courses_t*)node;
	free(destroy_node->course_name);
	free(destroy_node);
	return;
}

struct grades* grades_init() {
	grades *all_list = malloc(sizeof(struct grades));
	(all_list->student_list) = list_init(clone_student_node, 
		destroy_student_node);
	return all_list;
}

void grades_destroy(struct grades *grades) {
	list_destroy(grades->student_list);
	free(grades);
}

int grades_add_student(struct grades *grades, const char *name, int id) {
	struct node *students_it = list_begin(grades->student_list);
	if ((struct students_t*)list_get(students_it) != NULL) {
		int len_list = list_size(grades->student_list);
		for (int i=0; i<len_list; i++) {
			if (id == ((struct students_t*)list_get(students_it))->id) {
				return -1;
			}
			else {
				students_it = list_next(students_it);
			}
		}
	}
	students_t *tmp = create_student_node((char*)name,id);
	return list_push_back(grades->student_list,tmp);
}

int grades_add_grade(struct grades *grades, const char *name, int id, 
	int grade) {
	if (grades == NULL) {
		return -1;
	}
	if ((grade>100) || (grade<0)) {
		return -1;
	}
	struct node *students_it = list_begin(grades->student_list);
	size_t len_list = list_size(grades->student_list);
	for (int i=0; i<len_list; i++){
		students_t *tmp_student = ((struct students_t*)list_get(students_it));
		if (id == tmp_student->id){
			if ((tmp_student->list_courses) == NULL){
				tmp_student->list_courses = list_init(clone_courses_node, 
					destroy_courses_node);
				courses_t *new_course = create_courses_node((char*)name,grade);
				if(!(list_push_back(tmp_student->list_courses, new_course))) {
					return 0;
				} else {
					return -1;
				}
			} else {
				struct node *course_it = list_begin(tmp_student->list_courses);
				courses_t *tmp_course;
				int len_list = list_size(tmp_student->list_courses);
					for (i=0; i<len_list; i++){
						tmp_course = ((struct courses_t*)list_get(course_it));
						if(!(strcmp(name, tmp_course->course_name))) {
							return -1;
						} else {
							course_it = list_next(course_it);
						}
					} 
					courses_t *new_course = create_courses_node((char*)name,
						grade);
					if (!(list_push_back(tmp_student->list_courses, 
						new_course))) {
						return 0;
					} else {
						return -1;
					}
			}
		} else {
			students_it = list_next(students_it);
		}
	}
	return -1;
}

/*This function calculates the average of the course grades of
 *a specific student. Also, the variable 'out' will point the string 
 *of the student name.*/
float grades_calc_avg(struct grades *grades, int id, char **out) {
	if (grades == NULL) {
		return -1;
	}
	struct node *students_it = list_begin(grades->student_list);
	size_t len_list = list_size(grades->student_list);

	for (int i=0; i<len_list; i++) {
		students_t *tmp_student = ((struct students_t*)list_get(students_it));
		if (id == (tmp_student->id)) {
			char *name_student = (char*)malloc((strlen(((struct students_t*)
				list_get(students_it))->name)+1)*sizeof(char));
			strcpy(name_student,
				((struct students_t*)list_get(students_it))->name);
			(*out) = name_student;

			struct node *course_it = list_begin(((struct students_t*)
				list_get(students_it))->list_courses);
			float courses_sum = 0;
			float grades_sum = 0;

			size_t len_course = list_size(tmp_student->list_courses);
			if (len_course == 0) {
				return 0;
			}

			for (int j=0; j<len_course; j++){
				courses_sum += 1;
				grades_sum += ((struct courses_t*)list_get(course_it))->grade;
				course_it = list_next(course_it);
			}
 
			float average = grades_sum / courses_sum;
			return average;
		}
		students_it = list_next(students_it);
	}
	return -1;
}

/*This function prints the student name, ID number, course names and grades
 *for a specific student.*/
int grades_print_student(struct grades *grades, int id) {
	if (grades == NULL){
		return -1;
	}
	struct node *search_id = list_begin(grades->student_list);
	students_t *tmp_student;
	size_t len_list = list_size(grades->student_list);

	for (int i=0; i<len_list; i++){

		if ((((struct students_t*)(list_get(search_id)))->id) == id) {
			tmp_student = ((struct students_t*)list_get(search_id));
			struct node *course_it = list_begin(tmp_student->list_courses);
	
			printf("%s %d:",tmp_student->name, tmp_student->id);
			
			int len_list_course = list_size(tmp_student->list_courses);
			if (len_list_course == 0){
				printf("\n");
				return 0;
			}
			for (int j=0; j<len_list_course-1; j++){
				printf(" %s %d,",
					((struct courses_t*)list_get(course_it))->course_name,
					((struct courses_t*)list_get(course_it))->grade);
				course_it = list_next(course_it);
			}
			printf(" %s %d",
				((struct courses_t*)list_get(course_it))->course_name,
				((struct courses_t*)list_get(course_it))->grade);
			printf("\n");
			return 0;
		}
		search_id = list_next(search_id);		
	}
	return -1;
}

/*This function prints the student's name, ID number, course names and grades
 *for each of the students on the list, using the 'grades_print_student'
 *function that performs this for a single student.*/
int grades_print_all(struct grades *grades) {	
	if (grades == NULL) {
		return -1;
	}
	struct node *student_it = list_begin(grades->student_list);
	size_t len_list = list_size(grades->student_list);

	for(int i=0; i<len_list; i++){
		grades_print_student(grades,
			((struct students_t*)list_get(student_it))->id);
		student_it = list_next(student_it);
	}
	return 0;
}