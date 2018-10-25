#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum
{

	PAN,
	name,
	expire_date,
	service_code,
	discretionary,
	end

}Track_Field;


typedef struct
{
	char format_code;
	char PAN[20];
	char name[27];
	char expire_date[5];
	char service_code[4];
	char discretionary_data[80];
}Track_1_Container;

typedef struct
{
	char PAN[20];
	char expire_date[5];
	char service_code[4];
	char discretionary_data[40];
}Track_2_Container;


int extract_track_1_data(char* track_1_data, Track_1_Container* track_1_info);
int extract_track_1_data_v2(char* track_1_data, Track_1_Container* track_1_info);
int extract_track_2_data(char* track_2_data, Track_2_Container* track_2_info);


int main()
{

	char *track_1_data = "%B4815881002861896^YATES/EUGENE JOHN^99019821000123456789?";
	char *track_2_data = ";4815881002861896=990110123456700000?";


	Track_1_Container *track_1_info = malloc(sizeof(Track_1_Container));
	if (extract_track_1_data_v2(track_1_data, track_1_info) != 1)
	{
		/* error */
		printf("error\n");
	}

	Track_2_Container *track_2_info = malloc(sizeof(Track_2_Container));
	if (extract_track_2_data(track_2_data, track_2_info) != 1)
	{
		/* error */
		printf("error\n");
	}


	printf("==========================================================\n");
	printf("                       Track 1 data                       \n");
	printf("==========================================================\n");
	printf("PAN: %s\n", track_1_info->PAN);
	printf("Name: %s\n", track_1_info->name);
	printf("Expiration Data: %s\n", track_1_info->expire_date);
	printf("Service code: %s\n", track_1_info->service_code);
	printf("Discretionary data: %s\n", track_1_info->discretionary_data);



	printf("==========================================================\n");
	printf("                       Track 2 data                       \n");
	printf("==========================================================\n");
	printf("PAN: %s\n", track_2_info->PAN);
	printf("Expiration Data: %s\n", track_2_info->expire_date);
	printf("Service code: %s\n", track_2_info->service_code);
	printf("Discretionary data: %s\n", track_2_info->discretionary_data);



	system("pause");


	return 0;
}



int extract_track_1_data(char* track_1_data, Track_1_Container* track_1_info)
{

	size_t PAN_len = 0;
	size_t name_len = 0;
	size_t expiration_date_len = 0;
	size_t service_code_len = 0;
	size_t discretionary_len = 0;

	if (strlen(track_1_data) <= 2 || track_1_data[0] != '%' || track_1_data[1] != 'B')
	{
		printf("Error: Invalid Magnetic Stripe Track 1 Data.\n");
		return 0;
	}
	if (track_1_data[1] != 'B')
	{
		printf("Error: Invalid foramt code.\n");
		return 0;
	}


	size_t field_start_index = 2;
	Track_Field state = PAN;

	for (int i = 2; i < strlen(track_1_data); i++)
	{
		switch (state)
		{
		case PAN:
			if (track_1_data[i] <= '9' && track_1_data[i] >= '0')
			{
				PAN_len += 1;
			}
			else if (track_1_data[i] == '^')
			{
				if (PAN_len > 19 || PAN_len < 12)
				{
					printf("Error: Invalid PAN length.\n");
					return 0;
				}
				/* fill PAN data */
				memcpy(track_1_info->PAN, track_1_data + field_start_index, PAN_len);
				memset(track_1_info->PAN + PAN_len, 0, 1);

				field_start_index = i + 1; // name field start
				state = name;
			}
			else
			{
				printf("Error: Invalid char in PAN field.\n");
				return 0;
			}
			break;
		case name:
			if (track_1_data[i] != '^')
			{
				name_len += 1;
			}
			else if (track_1_data[i] == '^')
			{
				if (name_len > 26 || name_len < 2)
				{
					printf("Error: Invalid name length.\n");
					return 0;
				}
				/* fill name data */
				memcpy(track_1_info->name, track_1_data + field_start_index, name_len);
				memset(track_1_info->name + name_len, 0, 1);

				field_start_index = i + 1; // expiration data field start
				state = expire_date;
			}
			else
			{
				printf("Error: Invalid char in name field.\n");
				return 0;
			}
			break;
		case expire_date:
			if (track_1_data[i] >= '0' && track_1_data[i] <= '9')
			{
				expiration_date_len += 1;
				if (expiration_date_len == 4)
				{
					/* fill expiration date data */
					memcpy(track_1_info->expire_date, track_1_data + field_start_index, expiration_date_len);
					memset(track_1_info->expire_date + expiration_date_len, 0, 1);

					field_start_index = i + 1; // service code data field start
					state = service_code;
				}
			}
			else if (track_1_data[i] == '^')
			{
				if (expiration_date_len != 0)
				{
					printf("Error: Invalid expiration date data.\n");
				}
				else
				{
					memset(track_1_info->expire_date + expiration_date_len, 0, 1); // set 0

					field_start_index = i + 1; // service code data field start
					state = service_code;
				}
			}
			else 
			{
				printf("Error: Invalid char in expire date field.\n");
				return 0;
			}
			break;
		case service_code:
			if (track_1_data[i] >= '0' && track_1_data[i] <= '9')
			{
				service_code_len += 1;
				if (service_code_len == 3)
				{
					/* fill service code data */
					memcpy(track_1_info->service_code, track_1_data + field_start_index, service_code_len);
					memset(track_1_info->service_code + service_code_len, 0, 1);

					field_start_index = i + 1; // discretionary data field start
					state = discretionary;
				}
			}
			else if (track_1_data[i] == '^')
			{
				if (service_code_len != 0)
				{
					printf("Error: Invalid service code data.\n");
				}
				else
				{
					memset(track_1_info->service_code + service_code_len, 0, 1); // set 0

					field_start_index = i + 1; // discretionary data field start
					state = discretionary;
				}
			}
			else
			{
				printf("Error: Invalid char in service code field.\n");
				return 0;
			}
			break;
		case discretionary:
			if (track_1_data[i] != '?')
			{
				discretionary_len += 1;
			}
			else if (track_1_data[i] == '?')
			{
				/* fill discretionary data */
				memcpy(track_1_info->discretionary_data, track_1_data + field_start_index, discretionary_len);
				memset(track_1_info->discretionary_data + discretionary_len, 0, 1);

				state = end;
			}
			break;
		default:
			break;
		}

		if (i > 78)
		{
			printf("Error: Invalid track 1 data length.\n");
			return 0;
		}
	}

	return 1; /* successfully */

}


int extract_track_2_data(char* track_2_data, Track_2_Container* track_2_info)
{

	size_t PAN_len = 0;
	size_t expiration_date_len = 0;
	size_t service_code_len = 0;
	size_t discretionary_len = 0;


	/* Track 2 */
	if (strlen(track_2_data) <= 2 || track_2_data[0] != ';')
	{
		printf("Error: Invalid Magnetic Stripe Track 2 Data.\n");
		return 0;
	}


	size_t field_start_index = 1;
	Track_Field state_2 = PAN;

	for (int i = 1; i < strlen(track_2_data); i++)
	{
		switch (state_2)
		{
			case PAN:
				if (track_2_data[i] <= '9' && track_2_data[i] >= '0')
				{
					PAN_len += 1;
				}
				else if (track_2_data[i] == '=')
				{
					if (PAN_len > 19 || PAN_len < 12)
					{
						printf("Error: Invalid PAN length.\n");
						return 0;
					}
					/* fill PAN data */
					memcpy(track_2_info->PAN, track_2_data + field_start_index, PAN_len);
					memset(track_2_info->PAN + PAN_len, 0, 1);

					field_start_index = i + 1; // name field start
					state_2 = expire_date;
				}
				else
				{
					printf("Error: Invalid char in PAN field.\n");
					return 0;
				}
				break;
			case expire_date:
				if (track_2_data[i] >= '0' && track_2_data[i] <= '9')
				{
					expiration_date_len += 1;
					if (expiration_date_len == 4)
					{
						/* fill expiration date data */
						memcpy(track_2_info->expire_date, track_2_data + field_start_index, expiration_date_len);
						memset(track_2_info->expire_date + expiration_date_len, 0, 1);

						field_start_index = i + 1; // service code data field start
						state_2 = service_code;
					}
				}
				else if (track_2_data[i] == '=')
				{
					if (expiration_date_len != 0)
					{
						printf("Error: Invalid expiration date data.\n");
					}
					else
					{
						memset(track_2_info->expire_date + expiration_date_len, 0, 1); // set 0

						field_start_index = i + 1; // service code data field start
						state_2 = service_code;
					}
				}
				else
				{
					printf("Error: Invalid char in expire date field.\n");
					return 0;
				}
				break;
			case service_code:
				if (track_2_data[i] >= '0' && track_2_data[i] <= '9')
				{
					service_code_len += 1;
					if (service_code_len == 3)
					{
						/* fill service code data */
						memcpy(track_2_info->service_code, track_2_data + field_start_index, service_code_len);
						memset(track_2_info->service_code + service_code_len, 0, 1);

						field_start_index = i + 1; // discretionary data field start
						state_2 = discretionary;
					}
				}
				else if (track_2_data[i] == '=')
				{
					if (service_code_len != 0)
					{
						printf("Error: Invalid service code data.\n");
					}
					else
					{
						memset(track_2_info->service_code + service_code_len, 0, 1); // set 0

						field_start_index = i + 1; // service code data field start
						state_2 = discretionary;
					}
				}
				else
				{
					printf("Error: Invalid char in service code field.\n");
					return 0;
				}
				break;
			case discretionary:
				if (track_2_data[i] != '?')
				{
					discretionary_len += 1;
				}
				else if (track_2_data[i] == '?')
				{

					/* fill discretionary data */
					memcpy(track_2_info->discretionary_data, track_2_data + field_start_index, discretionary_len);
					memset(track_2_info->discretionary_data + discretionary_len, 0, 1);

					state_2 = end;
				}
				break;
			default:
				break;
		}
		if (i > 42)
		{
			printf("Error: Invalid track 2 data length.\n");
			return 0;
		}
	}

	return 1; /* successfully */

}


int extract_track_1_data_v2(char* track_1_data, Track_1_Container* track_1_info)
{

	char start_sentinel = '%';
	char end_sentinel = '?';
	char field_separator = '^';

	size_t start_sentinel_index = 0;
	size_t end_sentinel_index = 0;

	size_t separator_indices[4];
	size_t separator_num = 0;

	for (int i = 0; i < strlen(track_1_data); i++)
	{
		if (track_1_data[i] == start_sentinel)
		{
			start_sentinel_index = i;
		}
		if (track_1_data[i] == end_sentinel)
		{
			end_sentinel_index = i;
		}

		if (track_1_data[i] == field_separator)
		{
			separator_indices[separator_num] = i;
			separator_num += 1;
		}
		if (separator_num > 4)
		{
			printf("Invalid number of separator '%c'.\n", field_separator);
			return 0;
		}
	}


	if (start_sentinel_index != 0 || (end_sentinel_index != (strlen(track_1_data) - 1)))
	{
		printf("Invalid start sentinel or end sentinel.\n");
		return 0;
	}


	if (separator_num == 2)
	{
		if ((separator_indices[0] - start_sentinel_index - 1 - 1) < 12 || separator_indices[0] > 22)
		{
			printf("Error: Invalid PAN length.\n");
			return 0;
		}

		track_1_info->format_code = track_1_data[0];
		memcpy(track_1_info->PAN, track_1_data + 2, separator_indices[0] - 2);
		memset(track_1_info->PAN + separator_indices[0] - 2, 0, 1);

		if ((separator_indices[1] - separator_indices[0] - 1) < 2 || (separator_indices[1] - separator_indices[0] - 1) > 26)
		{
			printf("Error: Invalid name length.\n");
			return 0;
		}
		memcpy(track_1_info->name, track_1_data + separator_indices[0] + 1, separator_indices[1] - separator_indices[0] - 1);
		memset(track_1_info->name + separator_indices[1] - separator_indices[0] - 1, 0, 1);

		memcpy(track_1_info->expire_date, track_1_data + separator_indices[1] + 1, 4);
		memset(track_1_info->expire_date + 4, 0, 1);

		memcpy(track_1_info->service_code, track_1_data + separator_indices[1] + 5, 3);
		memset(track_1_info->service_code + 3, 0, 1);

		memcpy(track_1_info->discretionary_data, track_1_data + separator_indices[1] + 8, end_sentinel_index - separator_indices[1] - 7 - 1);
		memset(track_1_info->discretionary_data + end_sentinel_index - separator_indices[1] - 7 - 1, 0, 1);
	}
	else if (separator_num == 3)
	{

		if (separator_indices[0] < 14 || separator_indices[0] > 22)
		{
			printf("Error: Invalid PAN length.\n");
			return 0;
		}

		track_1_info->format_code = track_1_data[0];
		memcpy(track_1_info->PAN, track_1_data + 2, separator_indices[0] - 2);
		memset(track_1_info->PAN + separator_indices[0] - 2, 0, 1);

		if ((separator_indices[1] - separator_indices[0] - 1) < 2 || (separator_indices[1] - separator_indices[0] - 1) > 26)
		{
			printf("Error: Invalid name length.\n");
			return 0;
		}

		memcpy(track_1_info->name, track_1_data + separator_indices[0] + 1, separator_indices[1] - separator_indices[0] - 1);
		memset(track_1_info->name + separator_indices[1] - separator_indices[0] - 1, 0, 1);


		if (separator_indices[2] - separator_indices[1] == 5)
		{
			memcpy(track_1_info->expire_date, track_1_data + separator_indices[1] + 1, 4);
			memset(track_1_info->expire_date + 4, 0, 1);

			memset(track_1_info->service_code, 0, 1);

			memcpy(track_1_info->discretionary_data, track_1_data + separator_indices[2] + 1, end_sentinel_index - separator_indices[2] - 1);
			memset(track_1_info->discretionary_data + end_sentinel_index - separator_indices[2] - 1, 0, 1);
		}
		else if (separator_indices[2] - separator_indices[1] == 1)
		{
			memset(track_1_info->expire_date, 0, 1);

			memcpy(track_1_info->service_code, track_1_data + separator_indices[2] + 1, 3);
			memset(track_1_info->service_code + 3, 0, 1);

			memcpy(track_1_info->discretionary_data, track_1_data + separator_indices[2] + 4, end_sentinel_index - separator_indices[2] - 4);
			memset(track_1_info->discretionary_data + end_sentinel_index - separator_indices[2] - 4, 0, 1);
		}
		else
		{
			printf("Error: Invalid expire date length or service code length.\n");
			return 0;
		}



	}
	else if (separator_num == 4 )
	{
		if (separator_indices[0] < 14 || separator_indices[0] > 22)
		{
			printf("Error: Invalid PAN length.\n");
			return 0;
		}

		track_1_info->format_code = track_1_data[0];
		memcpy(track_1_info->PAN, track_1_data + 2, separator_indices[0] - 2);
		memset(track_1_info->PAN + separator_indices[0] - 2, 0, 1);

		if ((separator_indices[1] - separator_indices[0] - 1) < 2 || (separator_indices[1] - separator_indices[0] - 1) > 26)
		{
			printf("Error: Invalid name length.\n");
			return 0;
		}
		memcpy(track_1_info->name, track_1_data + separator_indices[0] + 1, separator_indices[1] - separator_indices[0] - 1);
		memset(track_1_info->name + separator_indices[1] - separator_indices[0] - 1, 0, 1);

		if (separator_indices[2] - separator_indices[1] == 1)
		{
			memset(track_1_info->expire_date, 0, 1);
		}
		else
		{
			printf("Error: Invalid expiration date field length.\n");
			return 0;
		}
		if (separator_indices[3] - separator_indices[2] == 1)
		{
			memset(track_1_info->service_code, 0, 1);
		}
		else
		{
			printf("Error: Invalid service code field length.\n");
			return 0;
		}
		
		memcpy(track_1_info->discretionary_data, track_1_data + separator_indices[3] + 1, end_sentinel_index - separator_indices[3] - 1);
		memset(track_1_info->discretionary_data + end_sentinel_index - separator_indices[3] - 1, 0, 1);

	}
	else
	{
		printf("Invalid number of separator '%c'.\n", field_separator);
		return 0;
	}

	return 1;

}