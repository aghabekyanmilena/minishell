/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atseruny <atseruny@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:34:35 by miaghabe          #+#    #+#             */
/*   Updated: 2025/07/15 18:46:18 by atseruny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/built_in.h"

static void	print_export_error(char *arg)
{
	g_err_no = 1;
	write(2, "export: `", 9);
	write(2, arg, ft_strlen(arg));
	write(2, "`: not a valid identifier\n", 27);
}

int	find_env_var_index(char **env, char *name)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(name);
	while (env && env[i])
	{
		if (!ft_strncmp(env[i], name, len))
			return (i);
		i++;
	}
	return (-1);
}

static int is_valid_var_name(char *name)
{
	int	i;

	i = 1;
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	while (name[i] && name[i] != '=' && name[i] != '+')
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	no_arg_case(char **args, t_data *data)
{
	char	*equal;
	char	**env_copy;
	int		i;
	int		j;
	int		key_len;

	if (!args[1])
	{
		i = 0;
		while (data->env && data->env[i])
			i++;
		env_copy = malloc(sizeof(char *) * (i + 1));
		if (!env_copy)
			return ;
		j = 0;
		while (j < i)
		{
			env_copy[j] = ft_strdup(data->env[j]);
			j++;
		}
		env_copy[i] = NULL;
		sort_env(env_copy);
		j = 0;
		while (env_copy[j])
		{
			equal = ft_strchr(env_copy[j], '=');
			if (equal)
			{
				key_len = equal - env_copy[j];
				write(1, "declare -x ", 11);
				write(1, env_copy[j], key_len);
				write(1, "=\"", 2);
				write(1, equal + 1, ft_strlen(equal + 1));
				write(1, "\"\n", 2);
			}
			else
			{
				write(1, "declare -x ", 11);
				write(1, env_copy[j], ft_strlen(env_copy[j]));
				write(1, "\n", 1);
			}
			free(env_copy[j]);
			j++;
		}
		free(env_copy);
	}
}


int builtin_export(char **args, t_data *data)
{
	int		i;
	char	*equal_sign;
	char	*var_name;
	char	**new_env;
	char	*tmp;
	char	*tmp2;
	int		j;
	int		name_len;
	int		index;
	int		len;

	i = 1;
	no_arg_case(args, data);
	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
			return (print_export_error(args[i]), 1);
		equal_sign = ft_strchr(args[i], '=');
		if (!equal_sign)
		{
			var_name = ft_strdup(args[i]);
			if (!var_name)
				return (1);
		}
		else
		{
			if (*(equal_sign - 1) == '+')
				name_len = equal_sign - args[i] - 1;
			else
				name_len = equal_sign - args[i];
			var_name = malloc(name_len + 1);
			if (!var_name)
				return (1);
			ft_strncpy(var_name, args[i], name_len);
			var_name[name_len] = '\0';
		}
		if (ft_strcmp(var_name, "_") == 0)
		{
			i++;
			continue;
		}
		index = find_env_var_index(data->env, var_name);
		if (index >= 0)
		{
			if (equal_sign && *(equal_sign - 1) == '+')
			{
				tmp = ft_substr(data->env[index], name_len + 1, ft_strlen(data->env[index]) - name_len - 1);
				free(data->env[index]);
				tmp2 = ft_strsjoin(var_name, tmp, '=');
				free(tmp);
				tmp = ft_substr(args[i], name_len + 2, ft_strlen(args[i]) - 2);
				data->env[index] = ft_strjoin(tmp2, tmp);
				free(tmp);
				free(tmp2);
			}
			else
			{
				free(data->env[index]);
				data->env[index] = ft_strdup(args[i]);
			}
		}
		else
		{
			len = 0;
			while (data->env && data->env[len])
				len++;
			new_env = malloc(sizeof(char *) * (len + 2));
			j = 0;
			while (j < len)
			{
				new_env[j] = data->env[j];
				j++;
			}
			if (equal_sign && *(equal_sign - 1) == '+')
			{
				tmp = ft_substr(args[i], name_len + 1, ft_strlen(args[i]) - name_len - 1);
				new_env[len] = ft_strjoin(var_name, tmp);
				free(tmp);
			}
			else
				new_env[len] = ft_strdup(args[i]);
			new_env[len + 1] = NULL;
			free(data->env);
			data->env = new_env;
		}
		i++;
		free(var_name);
	}
	g_err_no = 0;
	return (0);
}
